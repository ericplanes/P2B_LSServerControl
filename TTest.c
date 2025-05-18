#include <xc.h>
#include <pic18f4321.h>
#include "TAD_SIO.h"
#include "TEEPROM.h"
#include "TRAM.h"
#include "TTemperature.h"
#include "TMenu.h"
#include "TI2C.h"

void print(const BYTE *log);
void print_iterator(BYTE d, BYTE u);
void println(void);
void print_number(BYTE d, BYTE u);

static BYTE iteration = 0;
static BYTE buffer[TIMESTAMP_SIZE];
static BYTE temperature = 1;
static SYS_STATUS status = SYS_STATUS_OFF;

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void TEST_Init_PerifericsSimpleTest(void)
{
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

    // Write first log of the EEPROM
    SIO_PrintString("Writing 12345678901234 on the EEPROM...\r\n");
    while (EEPROM_StoreLog("12345678901234") == FALSE)
        ;

    // Read stored log of the EEPROM
    SIO_PrintString("EEPROM stored a: ");
    BYTE eeprom[15];
    while (EEPROM_ReadLog(0, eeprom) == FALSE)
        ;
    SIO_PrintString(eeprom);
    println();
    println();

    // Test timers
}

/*
 * Every second, prints EEPROM and RAM stored values. Every 10 seconds, updates the system state.
 */
void TEST_print_status(void)
{
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
