#include <xc.h>
#include <pic18f4321.h>
#include "TAD_SIO.h"
#include "TEEPROM.h"
#include "TRAM.h"
#include "TTemperature.h"

void wait1s(void);
void print(const BYTE *log);

static BYTE iteration = 0;
static BYTE buffer[TIMESTAMP_SIZE];
static BYTE temperature = 1;
static SYS_STATUS status = SYS_STATUS_OFF;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/*
 * Every second, prints EEPROM and RAM stored values. Every 10 seconds, updates the system state.
 */
void TEST_print_results(void)
{
    // Wait before starting
    wait1s();

    // Prepare testing
    set_default_config(void);

    // Check RAM stored temperatures
    SIO_PrintString("\n-------- RAM (Temperatures) --------\n");
    temperature = RAM_Read();
    while (temperature != 0)
    {
        temperature = RAM_Read();
        SIO_PrintString("\t" + i + ": " + temperature + "\n");
    }

    // Check EEPROM stored logs
    SIO_PrintString("\n----------- EEPROM (Logs) ----------\n");
    BYTE storedLogs = EEPROM_GetAmountOfStoredLogs();
    for (int i = 0; i < storedLogs; i++)
    {
        SIO_PrintString("\t" + i + ": \n");
        EEPROM_ReadLog(i, buffer);
        print(buffer);
    }

    // Increase iteration value
    iteration++;

    // Change system state if needed (each 10 seconds)
    if (iteration % 10 == 0)
    {
        if (status == SYS_STATUS_CRIT)
        {
            status = SYS_STATUS_OFF;
        }
        status++;
        TEMP_SimulateState(status);
    }
}

/* =======================================
 *         PRIVATE FUNCTION BODIES
 * ======================================= */
void wait1s()
{
    while (PORTBbits.RB1)
        ; // Wait for alarm flag (falling edge on RB1)
    DS3231_LimpiarFlagAlarma1();
}

void print(const BYTE *log)
{
    for (int i = 0; i < TIMESTAMP_SIZE && log[i] != '\n'; ++i)
    {
        SIO_SafePrint(log[i]);
    }
    SIO_PrintString("\r\n");
}
