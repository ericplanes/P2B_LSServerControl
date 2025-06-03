#include "TMenu.h"
#include "TAD_SIO.h"
#include "TJoystick.h"
#include "TAD_TIMER.h"
#include "TRAM.h"
#include "TEEPROM.h"
#include "TI2C.h"
#include <string.h> // Make sure this is included

/* =======================================
 *           PRIVATE DEFINES
 * ======================================= */

#define MENU_STATE_INIT 0
#define MENU_STATE_WAIT_COMMAND 1
#define MENU_STATE_SEND_LOGS 2
#define MENU_STATE_SEND_GRAPH 3
#define MENU_STATE_UPDATE_TIME 4
#define MENU_STATE_RESET_SYSTEM 5
#define MENU_STATE_CHECK_TIMER 6

/* =======================================
 *              CONFIG STRUCT
 * ======================================= */

typedef struct
{
    BYTE initialTime[15]; // "hhmmssDDMMYYYY" + '\0'
    BYTE samplingTime;
    BYTE thresholds[3];
    BOOL isConfigured;
} MenuConfig;

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static MenuConfig config;
static BYTE menu_state = MENU_STATE_WAIT_COMMAND;
static BYTE command_buffer[35] = {0};
static BYTE log_buffer[TIMESTAMP_SIZE];
static BYTE logs_remaining = 0;
static BYTE current_log_section = 0;
static BYTE time_timer = TI_TEST;
static BYTE timer_sio = TI_SIO;
static BYTE hour, min;
static BYTE command;

/* =======================================
 *         PRIVATE FUNCTION HEADERS
 * ======================================= */

static void initialize_system_with_config(void);
static void send_timestamp_update(void);
static void reset_config(void);
static void send_end_of_line(void);
static BYTE prepare_command_and_get_next_state(BYTE command);
static void send_temperature(BYTE stored_temp);

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void MENU_Init(void)
{
    config.isConfigured = FALSE;
    config.samplingTime = 0;
    config.thresholds[0] = 0;
    config.thresholds[1] = 0;
    config.thresholds[2] = 0;
    config.initialTime[0] = '\0';

    menu_state = MENU_STATE_WAIT_COMMAND;
    TiNewTimer(&time_timer);
    TiResetTics(time_timer);

    TiNewTimer(&timer_sio);
    TiResetTics(timer_sio);
}

void MENU_Motor(void)
{
    switch (menu_state)
    {
    case MENU_STATE_WAIT_COMMAND:
        memset(command_buffer, 0x00, sizeof(command_buffer));
        command = SIO_GetCommandAndValue(command_buffer);
        menu_state = prepare_command_and_get_next_state(command);
        break;

    case MENU_STATE_INIT:
        initialize_system_with_config();
        menu_state = MENU_STATE_WAIT_COMMAND;
        break;

    case MENU_STATE_UPDATE_TIME:
        SIO_parse_SetTime(command_buffer, &hour, &min);
        I2C_UpdateTimestamp(hour, min);
        send_timestamp_update();
        menu_state = MENU_STATE_WAIT_COMMAND;
        break;

    case MENU_STATE_CHECK_TIMER: // default if no command was sent on MENU_STATE_WAIT_COMMAND
        if (I2C_OneMinutePassed())
        {
            send_timestamp_update();
        }
        else
        {
            BYTE joy = Joystick_GetDirection();
            if (joy != JOY_CENTER)
            {
                SIO_SendCharCua(joy);
                send_end_of_line();
            }
        }
        menu_state = MENU_STATE_WAIT_COMMAND;
        break;

    case MENU_STATE_SEND_LOGS:
        if (logs_remaining == 0)
        {
            SIO_SendCharCua(COMMAND_FINISH);
            send_end_of_line();
            menu_state = MENU_STATE_WAIT_COMMAND;
        }

        if (EEPROM_ReadLog(current_log_section, log_buffer) == TRUE)
        {
            SIO_SendCharCua(COMMAND_DATALOGS);
            SIO_SendString(log_buffer, TIMESTAMP_SIZE - 1);
            send_end_of_line();

            logs_remaining--;
            current_log_section = EEPROM_GetNextSection(current_log_section);
        }

        break;

    case MENU_STATE_SEND_GRAPH:
    {
        BYTE stored_temp = RAM_Read();
        if (stored_temp != 0x00)
        {
            send_temperature(stored_temp);
        }
        else
        {
            SIO_SendCharCua(COMMAND_FINISH);
            send_end_of_line();
            menu_state = MENU_STATE_WAIT_COMMAND;
        }
    }
    break;
    }
}

BOOL MENU_isConfigured(void)
{
    return config.isConfigured;
}

const BYTE *MENU_GetTMPThresholds(void)
{
    return config.thresholds;
}

BYTE MENU_GetSamplingTime(void)
{
    return config.samplingTime;
}

/* =======================================
 *         PRIVATE FUNCTION BODIES
 * ======================================= */

/*
 * Gets the command. If it's either LOGS, GRAPH, SET_TIME, or RESET, does the previous steps before the loop from the command.asm
 * If no command, it returns the next state as CHECK_TIMER.
 */
static BYTE prepare_command_and_get_next_state(BYTE command)
{
    switch (command)
    {
    case COMMAND_GET_LOGS:
        logs_remaining = EEPROM_GetAmountOfStoredLogs();
        current_log_section = EEPROM_GetFirstSection();
        return MENU_STATE_SEND_LOGS;

    case COMMAND_GET_GRAPH:
        RAM_PrepareReadFrom0();
        return MENU_STATE_SEND_GRAPH;

    case COMMAND_SET_TIME:
        return MENU_STATE_UPDATE_TIME;

    case COMMAND_RESET:
        reset_config();
        return MENU_STATE_WAIT_COMMAND;

    case COMMAND_INITIALIZE:
        return MENU_STATE_INIT;

    default:
        return MENU_STATE_CHECK_TIMER;
    }
}

static void initialize_system_with_config(void)
{
    BYTE hour, min, day, month, year;
    BYTE rate, low, mod, high, crit;

    SIO_parse_Initialize(command_buffer, &hour, &min, &day, &month, &year, &rate, &low, &mod, &high);

    config.samplingTime = rate;
    config.thresholds[0] = low;
    config.thresholds[1] = mod;
    config.thresholds[2] = high;
    config.isConfigured = TRUE;

    I2C_SetTimestamp(hour, min, 0, 1, day, month, year);
}

static void send_timestamp_update(void)
{
    static BYTE now[TIMESTAMP_SIZE];
    I2C_ReadTimestamp(now);
    SIO_SendCharCua(COMMAND_UPDATETIME);
    SIO_SendCharCua(now[0]);
    SIO_SendCharCua(now[1]);
    SIO_SendCharCua(':');
    SIO_SendCharCua(now[2]);
    SIO_SendCharCua(now[3]);
    send_end_of_line();
}

static void reset_config(void)
{
    config.isConfigured = FALSE;
    EEPROM_CleanMemory();
    RAM_Reset();
}

static void send_end_of_line(void)
{
    SIO_SendCharCua(EOC1);
    SIO_SendCharCua(EOC2);
}

static void send_temperature(BYTE stored_temp)
{
    while (TiGetTics(timer_sio) < 1) // Wait 2ms
        ;
    SIO_SendCharCua(COMMAND_DATAGRAPH);
    SIO_SendCharCua('0' + (stored_temp / 10));
    SIO_SendCharCua('0' + (stored_temp % 10));
    send_end_of_line();
    TiResetTics(timer_sio);
}
