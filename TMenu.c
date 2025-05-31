#include "TMenu.h"
#include "TAD_SIO.h"
#include "TJoystick.h"
#include "TAD_TIMER.h"
#include "TRAM.h"
#include "TEEPROM.h"
#include "TI2C.h"

/* =======================================
 *           PRIVATE DEFINES
 * ======================================= */

#define MENU_STATE_WAIT_INIT 0
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
static BYTE command_buffer[32];
static BYTE log_buffer[TIMESTAMP_SIZE];
static BYTE logs_remaining = 0;
static BYTE current_log_section = 0;
static BYTE time_timer = TI_TEST;
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

    menu_state = MENU_STATE_WAIT_INIT;
    TiNewTimer(&time_timer);
    TiResetTics(time_timer);
}

void MENU_Motor(void)
{
    switch (menu_state)
    {
    case MENU_STATE_WAIT_INIT:
        command = SIO_GetCommandAndValue(command_buffer);
        if (command == COMMAND_INITIALIZE)
        {
            initialize_system_with_config();
            menu_state = MENU_STATE_WAIT_COMMAND;
        }
        break;

    case MENU_STATE_WAIT_COMMAND:
        command = SIO_GetCommandAndValue(command_buffer);
        menu_state = prepare_command_and_get_next_state(command);
        break;

    case MENU_STATE_CHECK_TIMER:
        if (TiGetTics(time_timer) >= ONE_SECOND * 60)
        {
            send_timestamp_update();
            TiResetTics(time_timer);
        }
        else
        {
            BYTE joy = Joystick_GetDirection();
            if (joy != JOY_CENTER)
                SIO_SendCharCua(joy);
        }
        menu_state = MENU_STATE_WAIT_COMMAND;
        break;

    case MENU_STATE_SEND_LOGS:
        if (EEPROM_ReadLog(current_log_section, log_buffer) == TRUE)
        {
            SIO_SendCharCua(COMMAND_DATALOGS);
            SIO_SendString((char *)log_buffer, TIMESTAMP_SIZE - 1);
            send_end_of_line();

            logs_remaining--;
            current_log_section = EEPROM_GetNextSection(current_log_section);
        }

        if (logs_remaining == 0)
        {
            SIO_SendCharCua(COMMAND_FINISH);
            menu_state = MENU_STATE_WAIT_COMMAND;
        }
        break;

    case MENU_STATE_SEND_GRAPH:
    {
        BYTE stored_temp = RAM_Read();
        if (stored_temp != 0x00)
        {
            SIO_SendCharCua(stored_temp);
        }
        else
        {
            SIO_SendCharCua(COMMAND_FINISH);
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

const BYTE *MENU_GetInitialTimeString(void)
{
    return config.initialTime;
}

void MENU_TEST_SetDefaultConfig(void)
{
    config.isConfigured = TRUE;
    config.thresholds[0] = 20;
    config.thresholds[1] = 25;
    config.thresholds[2] = 30;
    config.samplingTime = 1;

    const BYTE *defaultTime = (const BYTE *)"00000001012025";
    for (BYTE i = 0; i < 14; i++)
        config.initialTime[i] = defaultTime[i];
    config.initialTime[14] = '\0';
}

/* =======================================
 *         PRIVATE FUNCTION BODIES
 * ======================================= */
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
        SIO_SendCharCua(COMMAND_DATAGRAPH);
        return MENU_STATE_SEND_GRAPH;

    case COMMAND_SET_TIME: // TODO: Address fix of I2C_SetTimestamp
        SIO_parse_SetTime(command_buffer, &hour, &min);
        I2C_SetTimestamp(hour, min, 0, 1, 1, 1, 0); // dummy date
        send_timestamp_update();
        return MENU_STATE_WAIT_COMMAND;

    case COMMAND_RESET:
        reset_config();
        return MENU_STATE_WAIT_COMMAND;

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
    send_timestamp_update();
}

static void send_timestamp_update(void)
{
    static BYTE now[TIMESTAMP_SIZE];
    I2C_ReadTimestamp(now);
    SIO_SendCharCua(COMMAND_UPDATETIME);
    SIO_SendString((char *)now, TIMESTAMP_SIZE - 1);
    send_end_of_line();
}

static void reset_config(void)
{
    MENU_Init();
    TiInit(); // The goal is to clear the stored timers, but should check if it's ok
    EEPROM_CleanMemory();
    RAM_Reset();
}

static void send_end_of_line(void)
{
    SIO_SendCharCua(EOC1);
    SIO_SendCharCua(EOC2);
}
