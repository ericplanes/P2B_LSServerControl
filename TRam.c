#include "TRAM.h"

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE writeStage = 0;

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Simulates storing a WORD (2 bytes) into RAM byte by byte.
 * Returns FALSE (ongoing) the first time and TRUE (done) after the second byte.
 */
BOOL RAM_StoreTemperature(WORD temperature)
{
    switch (writeStage)
    {
    case 0:
        // Simulate writing LSB
        // Ex: write_byte(address, (BYTE)(temperature & 0xFF));
        writeStage++;
        return FALSE;

    case 1:
        // Simulate writing MSB
        // Ex: write_byte(address + 1, (BYTE)(temperature >> 8));
        writeStage = 0;
        return TRUE;

    default:
        writeStage = 0;
        return FALSE;
    }

    void RAM_Init(void)
    {
        TRISD = 0xFF;         // DATA
        TRISBbits.TRISB0 = 0; // !WE
        TRISBbits.TRISB3 = 0; // !OE
        TRISCbits.TRISC0 = 0; // !RESET COUNTER
        TRISCbits.TRISC5 = 0; // COUNT (CLK)

        LATBbits.LATB0 = 1; // !WE
        LATBbits.LATB3 = 1; // !OE
        LATCbits.LATC0 = 1; // !RESET COUNTER
    }

    void RAM_Write(unsigned char data)
    {
        LATBbits.LATB0 = 0; // !WE -> RAM mode write (pins de la RAM en input)
        TRISD = 0x00;       // DATA -> output
        LATD = data;        // Write data to RAM

        // Wait for the write operation to complete

        TRISD = 0xFF;       // DATA -> input
        LATBbits.LATB0 = 1; // !WE -> RAM mode idle
        count();            // Count pulse
    }

    unsigned char RAM_Read(void)
    {
        TRISD = 0xFF;               // DATA -> input
        LATBbits.LATB3 = 0;         // !OE -> RAM mode read (pins de la RAM en output)
        unsigned char data = PORTD; // Llegim dades de la RAM

        // Wait for the write operation to complete

        LATBbits.LATB3 = 1; // !OE -> RAM mode idle
        count();            // Count pulse
        return data;
    }

    void RAM_DebugPrint(void)
    {
        unsigned char data = RAM_Read(); // Llegeix dades de la RAM
        char buffer[6];                  // per convertir valors

        SIO_PrintString("RAM: ");
        itoa(data, buffer, 10);
        SIO_PrintString(buffer);
        SIO_SafePrint('\r');
        SIO_SafePrint('\n');
    }

    void RAM_Reset(void)
    {
        int i = 0;
        while (i < 32767) // 32768 = 2^15
        {
            RAM_Write(0x00); // Escrivim 0 a la RAM
            i++;
        }
    }

    void count(void)
    {
        LATCbits.LATC5 = 1; // COUNT (CLK)
        //__delay_us(1); // Wait for the clock pulse
        LATCbits.LATC5 = 0; // COUNT (CLK)
    }

    void count_reset(void)
    {
        LATCbits.LATC0 = 0; // !RESET COUNTER
        //__delay_us(1); // Wait for the reset pulse
        LATCbits.LATC0 = 1; // !RESET COUNTER
    }

    void RAM_WriteIncremental(void)
    {
        count_reset(); // Asegurar que comenzamos en dirección 0
        for (unsigned char i = 0; i < 200; i++)
        {
            RAM_Write(i); // Escribir número creciente
        }
    }

    void RAM_ReadAndPrint100(void)
    {
        count_reset(); // Volver a dirección 0
        char buffer[6];
        for (unsigned char i = 0; i < 200; i++)
        {
            unsigned char val = RAM_Read();
            itoa(val, buffer, 10);
            SIO_PrintString("Addr ");
            itoa(i, buffer, 10);
            SIO_PrintString(buffer);
            SIO_PrintString(": ");
            itoa(val, buffer, 10);
            SIO_PrintString(buffer);
            SIO_PrintString("\r\n");
        }
    }
