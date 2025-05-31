#include <xc.h>
#include <pic18f4321.h>
#include "TAD_SIO.h"
#include "TEEPROM.h"
#include "TRAM.h"
#include "TTemperature.h"
#include "TMenu.h"
#include "TI2C.h"
#include "TAD_TIMER.h"
#include "TController.h"

void print(const BYTE *log);
void print_iterator(BYTE d, BYTE u);
void println(void);
void print_number(BYTE d, BYTE u);
void print_long_number(BYTE c, BYTE d, BYTE u);

static BYTE iteration = 0;
static BYTE buffer[TIMESTAMP_SIZE];
static BYTE temperature = 1;
static SYS_STATUS status = SYS_STATUS_OFF;
static BYTE timer = TI_TEST;
static BYTE eeprom_amount_of_logs;
static BYTE ram_amount = 0;
static BYTE ram_data = 0;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/*
 * Method to be called at the beginning of the execution.
 * Tests Read and Write for: RAM, EEPROM, I2C.
 * Tests the Timer.
 */
void TEST_Init_PerifericsSimpleTest(void)
{
    /*
    // Starting message
    println();
    println();
    SIO_PrintString(" ----> INIT TEST AT DATE (I2C): ");
    I2C_TEST_PrintTimestamp();
    println();

    // Configure to start testing
    SIO_PrintString("Prepare fake config on menu...\r\n");
    MENU_TEST_SetDefaultConfig();
    println();

    // Update I2C time and print it
    SIO_PrintString("Writing timestamp 10/10/2010 10:10:10 and waiting 3 seconds...\r\n");
    I2C_SetTimestamp(10, 10, 10, 1, 10, 10, 10);
    I2C_TEST_Wait1S();
    I2C_TEST_Wait1S();
    I2C_TEST_Wait1S();

    SIO_PrintString("I2C Timestamp after 3 seconds: ");
    I2C_TEST_PrintTimestamp();
    println();

    // Write first value of the RAM
    SIO_PrintString("Writing a 25 on the RAM...\r\n");
    RAM_Write(25);

    // Read first value of the RAM and print it
    SIO_PrintString("RAM stored a: ");
    BYTE ram = RAM_TEST_Read_From_0();
    print_number(ram / 10, ram % 10);
    println();

    // Clean EEPROM
    SIO_PrintString("Cleaning EEPROM...\r\n");
    EEPROM_CleanMemory();

    // Write first log of the EEPROM
    SIO_PrintString("Writing 12345678901234 on the EEPROM...\r\n");
    while (EEPROM_StoreLog((BYTE *)"12345678901234") == FALSE)
        ;

    // Read stored log from the EEPROM
    SIO_PrintString("EEPROM stored a: ");
    BYTE eeprom[15];
    while (EEPROM_ReadLog(0, eeprom) == FALSE)
        ;
    SIO_PrintString((char *)eeprom);
    println();
    println();

    // Test timers
    TiNewTimer(&timer);
    TiResetTics(timer);
    SIO_PrintString("Testing timer, should print hour every 1 second, during 10 seconds...\r\n");
    for (int i = 0; i < 10;)
    {
        if (TiGetTics(timer) > ONE_SECOND)
        {
            TiResetTics(timer);
            print_iterator((BYTE)i / 10, (BYTE)i % 10);
            I2C_TEST_PrintTimestamp();
            i++;
        }
    }
    println();
    SIO_PrintString("---- END OF INITIAL TESTING ---- \r\n");
    //*/
}

/*
 * Every second, prints EEPROM and RAM stored values. Every 10 seconds, updates the system state.
 */
void TEST_print_status(void)
{
    /*
    if (TiGetTics(timer) < ONE_SECOND * 2 || EEPROM_CanBeUsed() == FALSE) // Wait 2 seconds per iteration and make sure that EEPROM available
    {
        return;
    }

    status++;
    TEMP_TEST_SimulateState(status);
    if (status == SYS_STATUS_CRIT)
        status = SYS_STATUS_OFF;
    TiResetTics(timer);

    // Print current CTR state
    SIO_PrintString((char *)CTR_TEST_GetInfo());

    // Print EEPROM status
    eeprom_amount_of_logs = EEPROM_GetAmountOfStoredLogs();
    SIO_PrintString("EEPROM AMOUNT OF STORED LOGS: ");
    print_number(eeprom_amount_of_logs / 10, eeprom_amount_of_logs % 10);
    println();

    // Print EEPROM stored values
    TiResetTics(timer);
    for (BYTE i = 0; i < eeprom_amount_of_logs;)
    {
        if (TiGetTics(timer) > ONE_SECOND * 2) // Print every 2 secons in case something goes wrong
        {
            SIO_PrintString("Inside the eeprom reading from the test...");
            TiResetTics(timer);
        }

        if (EEPROM_ReadLog(i, buffer) == TRUE)
        {
            SIO_PrintString((char *)buffer);
            println();
            i++;
        }
    }

    // Print RAM values
    ram_amount = 0;
    ram_data = RAM_TEST_Read_From_0();
    TiResetTics(timer);

    while (ram_data != 0 && ram_data != 0x00)
    {
        ram_amount++;
        ram_data = RAM_Read();

        // Debug option just in case
        if (TiGetTics(timer) > ONE_SECOND * 5)
        {
            SIO_PrintString("Computing amount of RAM values (5 seconds passed)...\r\n");
            TiResetTics(timer);
            if (ram_amount > 200) // Safe break in case the RAM gets blocked
                break;
        }
    }

    SIO_PrintString("RAM has this amount of stored values: ");
    print_long_number(ram_amount / 100, (ram_amount / 10) % 10, ram_amount % 10);
    println();
    // */
}

/* =======================================
 *         PRIVATE FUNCTION BODIES
 * ======================================= */
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

void print_long_number(BYTE c, BYTE d, BYTE u)
{
    SIO_PrintString("  ");
    SIO_SafePrint('0' + c);
    SIO_SafePrint('0' + d);
    SIO_SafePrint('0' + u);
    SIO_PrintString("\r\n");
}

void print_number(BYTE d, BYTE u)
{
    SIO_PrintString("  ");
    SIO_SafePrint('0' + d);
    SIO_SafePrint('0' + u);
    SIO_PrintString("\r\n");
}

void println(void)
{
    SIO_PrintString("\r\n");
}
