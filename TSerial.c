#include "TSerial.h"
#include <xc.h>
#include <pic18f4321.h>

/* =======================================
 *          CONFIGURATION CONSTANTS
 * ======================================= */

#define UART_TX_CONFIG 0x24 // TX enable, async, BRGH=1
#define UART_RX_CONFIG 0x90 // Serial enable, RX enable
#define BAUD_DIVISOR 10

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */
static void debug_print_char(BYTE character)
{
    while (Serial_CanSendChar() == FALSE)
        ; // Do nothing
    Serial_SendChar(character);
}

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void Serial_Init(void)
{
    // Configure RC6 (TX) and RC7 (RX) as inputs
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;

    TXSTAbits.BRGH = 1;    // High Baud Rate Select bit
    BAUDCONbits.BRG16 = 0; // Use 8 bits Baud Rate Generator
    SPBRG = 64;            // For 9600 bauds with Fosc = 10 MHz

    TXSTAbits.SYNC = 0; // Async
    TXSTAbits.TXEN = 1; // Transmission enable
    RCSTAbits.SPEN = 1; // Serial Port enable
    RCSTAbits.CREN = 1; // Reception enable
}

BOOL Serial_CanReadChar(void)
{
    return (PIR1bits.RCIF == 1) ? TRUE : FALSE;
}

BOOL Serial_CanSendChar(void)
{
    return (PIR1bits.TXIF == 1) ? TRUE : FALSE;
}

BYTE Serial_ReadChar(void)
{
    return RCREG;
}

void Serial_SendChar(BYTE value)
{
    TXREG = value;
}

void Serial_DebugPrintString(const BYTE *text)
{
    while (*text != '\0')
    {
        debug_print_char(*text++);
    }
}
