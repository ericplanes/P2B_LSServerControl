#ifndef TAD_JOY_H
#define TAD_JOY_H

// Inicialitza l'ADC i els pins RA0, RA1 i RB2
void JOY_Init(void);

// Llegeix el valor anal�gic (10 bits) del canal corresponent (0 = RA0, 1 = RA1)
unsigned int JOY_ReadAnalog(unsigned char canal);

// Llegeix l'estat digital del pin RB2 (0 o 1)
unsigned char JOY_ReadDigital(void);

void JOY_DebugPrint(void);

#endif
