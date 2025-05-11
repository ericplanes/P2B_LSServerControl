#ifndef _SIO_H_
#define _SIO_H_

#include <xc.h>
#include <pic18f4321.h>
#include <stdint.h>

#define _XTAL_FREQ 10000000 // 10 MHz

#define CONFIGURACIO_TXSTA 0x24 // Dades 8bits, TX EN, Asinc, BRGH=1
#define CONFIGURACIO_RCSTA 0x90 // EN canal serie, 8 bits, asinc, RX EN
#define DIVISOR_BAUDRATE 10
#define TRUE 1  // TRUE
#define FALSE 0 // FALSE

// FUNCIONS
void SIO_Init();
unsigned char SIO_RXAvail(void);
unsigned char SIO_TXAvail(void); // Available == 1
unsigned char SIO_GetChar(void);
void SIO_PutChar(unsigned char lletra);
void SIO_SafePrint(char lletra);
void SIO_PrintString(const char *text);
void itoa(unsigned int value, char *str, unsigned char base);

#endif