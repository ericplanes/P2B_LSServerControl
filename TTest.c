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
void int_to_ascii_digits(int number, char *buffer);

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
    set_default_config();

    // Check RAM stored temperatures
    SIO_PrintString("\n-------- RAM (Temperatures) --------\n");
    temperature = RAM_Read();
    int i = 0;
    while (temperature != 0 && i < 99)
    {
        temperature = RAM_Read();
        print_iterator(i / 10, i % 10);
        SIO_SafePrint('0' + temperature);
        println();
        i++;
    }
    i = 0;

    // Check EEPROM stored logs
    SIO_PrintString("\n----------- EEPROM (Logs) ----------\n");
    BYTE storedLogs = EEPROM_GetAmountOfStoredLogs();
    for (BYTE i = 0; i < storedLogs; i++)
    {
        EEPROM_ReadLog(i, buffer);
        print_iterator(i / 10, i % 10);
        print(buffer);
        println();
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
    for (BYTE i = 0; i < TIMESTAMP_SIZE && log[i] != '\n'; ++i)
    {
        SIO_SafePrint(log[i]);
    }
    SIO_PrintString("\r\n");
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

void int_to_ascii_digits(int number, char *buffer)
{
    int divisor = 10000;
    int index = 0;
    BOOL started = FALSE;

    if (number == 0)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }

    while (divisor > 0)
    {
        int digit = number / divisor;
        if (digit != 0 || started)
        {
            buffer[index++] = digit + '0';
            started = TRUE;
        }
        number %= divisor;
        divisor /= 10;
    }

    buffer[index] = '\0'; // null-terminate
}
