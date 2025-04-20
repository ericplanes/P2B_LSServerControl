#include "TSerial.h"
#include <xc.h>
#include <pic18f4321.h>

/* =======================================
 *          CONFIGURATION CONSTANTS
 * ======================================= */

#define UART_TX_CONFIG 0x24 // TX enable, async, BRGH=1
#define UART_RX_CONFIG 0x90 // Serial enable, RX enable
#define BAUD_DIVISOR 42

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void Serial_Init(void)
{
    // Configure RC6 (TX) and RC7 (RX) as inputs
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;

    // Baud rate configuration
    BAUDCONbits.BRG16 = 0; // 8-bit Baud Rate Generator
    BAUDCONbits.TXCKP = 0; // Idle state is high

    TXSTA = UART_TX_CONFIG;
    RCSTA = UART_RX_CONFIG;
    SPBRG = BAUD_DIVISOR;
    SPBRGH = 0;
}

BOOL Serial_CanReadChar(void)
{
    return (PIR1bits.RCIF == 1) ? TRUE : FALSE;
}

BYTE Serial_ReadChar(void)
{
    return RCREG;
}

BOOL Serial_CanSendChar(void)
{
    return (PIR1bits.TXIF == 1) ? TRUE : FALSE;
}

void Serial_SendChar(BYTE value)
{
    TXREG = value;
}
