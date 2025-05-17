#include "TController.h"
#include "TTimer.h"
#include "TTemperature.h"
#include "TMenu.h"
#include "TI2C.h"
#include "TRAM.h"
#include "TEEPROM.h"
#include "TAD_SIO.h"

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

static BYTE timer_id;
static BYTE wait_sample_time = 0;
static BYTE timestamp_buffer[TIMESTAMP_SIZE];
static BYTE temperature;

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void CTR_Init(void)
{
    timer_id = TiGetTimer();
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
    /*SIO_PrintString("Controller state: ");
    SIO_SafePrint('0' + motor_state);
    SIO_PrintString("\r\n");*/

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

        /*
        if (TiGetTics(timer_id) >= wait_sample_time * 1000)
        {
            TiResetTics(timer_id);
            motor_state = S_READ_TEMPERATURE;
        }
        */
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
