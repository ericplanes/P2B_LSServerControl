#include "TRAM.h"

#define MAX_RAM_ADDRESS 32768 // 2^15 addresses

/* =======================================
 *         PRIVATE FUNCTION HEADERS
 * ======================================= */

static void inc_address(void);

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

/*
 * Configures the initial state of the control bits, both from the RAM and the counter
 */
void RAM_Init(void)
{
    // Configure directions and initial state of pins
    TRISD = 0xFF;         // Port D com entrada (DATA)
    TRISBbits.TRISB0 = 0; // !WE (write enable)
    TRISBbits.TRISB3 = 0; // !OE (output enable)
    TRISCbits.TRISC0 = 0; // !RESET COUNTER
    TRISCbits.TRISC5 = 0; // COUNT (CLK)

    LATBbits.LATB0 = 1; // !WE inactiu
    LATBbits.LATB3 = 1; // !OE inactiu
    LATCbits.LATC0 = 1; // RESET inactiu
    RAM_Reset();        // Cleans all the previously recorded information
}

/*
 * Writes the RAM position with the byte information sent on data
 */
void RAM_Write(BYTE data)
{
    LATBbits.LATB0 = 0; // Activem !WE → mode escriptura
    TRISD = 0x00;       // Port D com a sortida
    LATD = data;        // Escrivim valor a la RAM

    TRISD = 0xFF;       // Port D com a entrada un altre cop
    LATBbits.LATB0 = 1; // Desactivem !WE → idle
    inc_address();      // Incrementem adreça
}

/*
 * Reads the RAM byte that we are pointing with the counter
 */
BYTE RAM_Read(void)
{
    TRISD = 0xFF;       // Port D com entrada
    LATBbits.LATB3 = 0; // Activem !OE → mode lectura
    BYTE data = PORTD;  // Llegim dades

    LATBbits.LATB3 = 1; // Desactivem !OE
    if (data != 0 && data != 0x00)
        inc_address(); // Incrementem adreça si no hem llegit un 0
    return data;
}

/*
 * As per requirement, writes all the available positions of the RAM to 0
 */
void RAM_Reset(void)
{
    RAM_PrepareReadFrom0();
    for (WORD i = 0; i < MAX_RAM_ADDRESS; i++)
    {
        RAM_Write(0x00); // Write 0 to all the positions
    }
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

/*
 * RAM COUNTER: Pulse to the CLK signal to increase the counter (RAM address) by 1.
 */
static void inc_address(void)
{
    LATCbits.LATC5 = 1;
    LATCbits.LATC5 = 0;
}

/*
 * RAM COUNTER: Pulse to the !Reset signal to make the address of the RAM = 0
 */
void RAM_PrepareReadFrom0(void)
{
    LATCbits.LATC0 = 0; // Activate Reset
    LATCbits.LATC0 = 1; // Deactivate Reset
}
