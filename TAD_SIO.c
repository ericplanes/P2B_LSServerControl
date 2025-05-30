#include "TAD_SIO.h"

void SIO_Init()
{
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;

    TXSTAbits.BRGH = 1;    // High Baud Rate Select bit (para mayor precisio?n)
    BAUDCONbits.BRG16 = 0; // Usar el Baud Rate Generator de 8 bits
    SPBRG = 64;            // Para 9600 baudios con Fosc = 10 MHz

    TXSTAbits.SYNC = 0; // Modo asi?ncrono
    TXSTAbits.TXEN = 1; // Habilitar transmisio?n
    RCSTAbits.SPEN = 1; // Habilitar puerto serie
    RCSTAbits.CREN = 1; // Habilitar recepció
}

unsigned char SIO_RXAvail()
{
    return ((PIR1bits.RCIF == 1) ? TRUE : FALSE);
}
unsigned char SIO_TXAvail()
{
    return ((PIR1bits.TXIF == 1) ? TRUE : FALSE);
}
unsigned char SIO_GetChar()
{
    return RCREG;
}
void SIO_PutChar(unsigned char lletra)
{
    TXREG = lletra;
}

void SIO_SafePrint(char lletra)
{ // NO COOPERATIU, NOMES PER DEBUGGING
    while (!SIO_TXAvail())
        ;
    SIO_PutChar(lletra);
}

void SIO_PrintString(const char *text)
{
    while (*text != '\0')
    {
        SIO_SafePrint(*text++);
    }
}

void itoa(unsigned int value, char *str, unsigned char base)
{
    char temp[6]; // Buffer temporal per invertir el n�mero (fins a 65535 = 5 xifres + \0)
    int i = 0;

    if (value == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (value > 0 && i < 5)
    {
        temp[i++] = (value % base) + '0';
        value /= base;
    }

    // Inverteix el resultat a str
    int j = 0;
    while (i > 0)
    {
        str[j++] = temp[--i];
    }
    str[j] = '\0'; // null terminador
}
