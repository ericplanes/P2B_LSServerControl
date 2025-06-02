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
static SYS_STATUS previous_status;
static BYTE motor_state;
static BOOL eeprom_can_write;

static BYTE timer_id = TI_CONT;
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
    previous_status = SYS_STATUS_OFF;
    motor_state = S_WAIT_CONFIG;
    eeprom_can_write = FALSE;
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
        if (TiGetTics(timer_id) >= wait_sample_time * ONE_SECOND)
        {
            TiResetTics(timer_id);
            motor_state = S_READ_TEMPERATURE;
        }
        if (!MENU_isConfigured()) // If reset, menu will not be configured anymore
        {
            motor_state = S_WAIT_CONFIG;
        }
        break;

    case S_READ_TEMPERATURE:
        previous_status = controller_status;
        controller_status = TEMP_GetState();
        temperature = TEMP_GetTemperature();

        if (controller_status == SYS_STATUS_CRIT && previous_status != SYS_STATUS_CRIT)
        {
            eeprom_can_write = TRUE;
            motor_state = S_READ_TIME;
        }
        else
            motor_state = S_WRITE_RAM;
        break;

    case S_READ_TIME:
        I2C_ReadTimestamp(timestamp_buffer);
        motor_state = S_WRITE_EEPROM;
        break;

    case S_WRITE_EEPROM:
        if (EEPROM_StoreLog(timestamp_buffer) == TRUE && eeprom_can_write == TRUE)
        {
            motor_state = S_WRITE_RAM;
            eeprom_can_write = FALSE;
        }
        break;

    case S_WRITE_RAM:
        RAM_Write(temperature);
        motor_state = S_WAIT_SAMPLETIME;
        break;
    }
}
