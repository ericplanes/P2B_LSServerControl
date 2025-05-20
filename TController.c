#include "TController.h"
#include <stdio.h>

/* =======================================
 *           PRIVATE CONSTANTS
 * ======================================= */
#define S_WAIT_CONFIG 0
#define S_WAIT_SAMPLETIME 1
#define S_READ_TEMPERATURE 2
#define S_READ_TIME 3
#define S_WRITE_EEPROM 4
#define S_WRITE_RAM 5

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static SYS_STATUS controller_status;
static BYTE motor_state;

static BYTE timer_id = 3;
static BYTE wait_sample_time = 0;
static BYTE timestamp_buffer[TIMESTAMP_SIZE];
static BYTE temperature;

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void CTR_Init(void)
{
    TiNewTimer(&timer_id);
    TiResetTics(timer_id);

    controller_status = SYS_STATUS_OFF;
    motor_state = S_WAIT_CONFIG;
}

SYS_STATUS CTR_GetStatus(void)
{
    return controller_status;
}

void CTR_Motor(void)
{
    switch (motor_state)
    {
    case S_WAIT_CONFIG:
        if (MENU_isConfigured())
        {
            wait_sample_time = MENU_GetSamplingTime();
            motor_state = S_WAIT_SAMPLETIME;
        }
        break;

    case S_WAIT_SAMPLETIME:
        motor_state = S_READ_TEMPERATURE;
        if (TiGetTics(timer_id) >= wait_sample_time * 1000)
        {
            TiResetTics(timer_id);
            motor_state = S_READ_TEMPERATURE;
        }
        break;

    case S_READ_TEMPERATURE:
        controller_status = TEMP_GetState();
        temperature = TEMP_GetTemperature();

        if (controller_status == SYS_STATUS_CRIT)
            motor_state = S_READ_TIME;
        else
            motor_state = S_WRITE_RAM;
        break;

    case S_READ_TIME:
        I2C_ReadTimestamp(timestamp_buffer);
        motor_state = S_WRITE_EEPROM;
        break;

    case S_WRITE_EEPROM:
        if (EEPROM_StoreLog(timestamp_buffer) == TRUE)
            motor_state = S_WRITE_RAM;
        break;

    case S_WRITE_RAM:
        RAM_Write(temperature);
        motor_state = S_WAIT_SAMPLETIME;
        break;
    }
}

/*
 * Gets the current information of the Controller and returns a string ready to be printed
 * Prints something like:
 * CONTROLLER
 *   Motor state: S_WAIT_CONFIG
 *   System state: SYS_STATUS_OFF
 */
BYTE *CTR_TEST_GetInfo(void)
{
    static char buffer[80];

    const char *motor_str = "";
    const char *status_str = "";

    switch (motor_state)
    {
    case S_WAIT_CONFIG:
        motor_str = "S_WAIT_CONFIG";
        break;
    case S_WAIT_SAMPLETIME:
        motor_str = "S_WAIT_SAMPLETIME";
        break;
    case S_READ_TEMPERATURE:
        motor_str = "S_READ_TEMPERATURE";
        break;
    case S_READ_TIME:
        motor_str = "S_READ_TIME";
        break;
    case S_WRITE_EEPROM:
        motor_str = "S_WRITE_EEPROM";
        break;
    case S_WRITE_RAM:
        motor_str = "S_WRITE_RAM";
        break;
    default:
        motor_str = "UNKNOWN";
        break;
    }

    switch (controller_status)
    {
    case SYS_STATUS_OFF:
        status_str = "SYS_STATUS_OFF";
        break;
    case SYS_STATUS_LOW:
        status_str = "SYS_STATUS_LOW";
        break;
    case SYS_STATUS_MOD:
        status_str = "SYS_STATUS_MOD";
        break;
    case SYS_STATUS_HIGH:
        status_str = "SYS_STATUS_HIGH";
        break;
    case SYS_STATUS_CRIT:
        status_str = "SYS_STATUS_CRIT";
        break;
    default:
        status_str = "UNKNOWN";
        break;
    }

    sprintf(buffer, "CONTROLLER\r\n  Motor state: %s\r\n  System state: %s\r\n", motor_str, status_str);

    return (BYTE *)buffer;
}
