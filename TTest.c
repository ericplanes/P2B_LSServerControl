#include <xc.h>
#include <pic18f4321.h>
#include "TAD_SIO.h"
#include "TEEPROM.h"
#include "TRAM.h"
#include "TTemperature.h"
#include "TMenu.h"
#include "TI2C.h"

void wait1s(void);
void print(const BYTE *log);
void print_iterator(BYTE d, BYTE u);
void println(void);

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
    // Prepare testing
    MENU_TEST_SetDefaultConfig();

    // Check RAM stored temperatures
    SIO_PrintString("\r\n-------- RAM (Temperatures) --------\r\n");
    temperature = RAM_Read();
    int i = 0;
    while (temperature != 0 && i < 99)
    {
        temperature = RAM_Read();
        print_iterator((BYTE)i / 10, (BYTE)i % 10);
        SIO_SafePrint('0' + temperature);
        println();
        i++;
    }
    i = 0;

    // Check EEPROM stored logs
    SIO_PrintString("\r\n----------- EEPROM (Logs) ----------\r\n");
    BYTE storedLogs = EEPROM_GetAmountOfStoredLogs();
    for (BYTE i = 0; i < storedLogs; i++)
    {
        if (EEPROM_ReadLog(i, buffer) == TRUE)
        {
            print_iterator((BYTE)i / 10, (BYTE)i % 10);
            print(buffer);
            println();
        }
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
        TEMP_TEST_SimulateState(status);
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
    for (BYTE i = 0; i < TIMESTAMP_SIZE && log[i] != '\n'; ++i)
    {
        SIO_SafePrint(log[i]);
    }
    println();
}

void print_iterator(BYTE d, BYTE u)
{
    SIO_PrintString("  ");
    SIO_SafePrint('0' + d);
    SIO_SafePrint('0' + u);
    SIO_PrintString(": ");
}

void println(void)
{
    SIO_PrintString("\r\n");
}
