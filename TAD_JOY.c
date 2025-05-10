#include "C:\Program Files\Microchip\xc8\v3.00\pic\include\xc.h"
#include "TAD_SIO.h"
#include "TAD_JOY.h"

void JOY_Init(void) {
    // Configura RA0 i RA1 com a entrades
    TRISAbits.TRISA0 = 1;
    TRISAbits.TRISA1 = 1;

    // Configura RB2 com a entrada digital
    TRISBbits.TRISB2 = 1;

    // Configura l'ADC:
    ADCON1 = 0b00001101; // RA0 i RA1 anal�gics, resta digitals
    ADCON2 = 0b10111110; // Right justified, Tacq = 4 TAD, Fosc/32
    ADCON0 = 0b00000101; // Per ara desactivat (es configura segons canal a cada lectura)
}

unsigned int JOY_ReadAnalog(unsigned char canal) {
    if (canal > 1) return 0; // Nom�s acceptem canal 0 (RA0) o 1 (RA1)

    ADCON0 = (canal << 2);   // Selecciona canal
    ADCON0bits.ADON = 1;     // Activa el m�dul ADC

    //__delay_us(5);           // Petita espera abans de convertir

    ADCON0bits.GO = 1;       // Inicia conversi�
    while (ADCON0bits.GO);   // Espera a que acabi

    return ((ADRESH << 8) + ADRESL); // Llegeix resultat (10 bits)
}

unsigned char JOY_ReadDigital(void) {
    return PORTBbits.RB2; // Llegeix digitalment RB2
}

void JOY_DebugPrint(void) {
    unsigned int val_RA0 = JOY_ReadAnalog(0);
    unsigned int val_RA1 = JOY_ReadAnalog(1);
    unsigned char val_RB2 = JOY_ReadDigital();

    char buffer[6]; // per convertir valors

    SIO_PrintString("RA0: ");
    itoa(val_RA0, buffer, 10);
    SIO_PrintString(buffer);

    SIO_PrintString(" | RA1: ");
    itoa(val_RA1, buffer, 10);
    SIO_PrintString(buffer);

    SIO_PrintString(" | RB2: ");
    buffer[0] = val_RB2 ? '1' : '0';
    buffer[1] = '\0';
    SIO_PrintString(buffer);

    SIO_SafePrint('\r');
    SIO_SafePrint('\n');
}