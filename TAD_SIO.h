#ifndef _SIO_H_
#define _SIO_H_

#include <xc.h>
#include <pic18f4321.h>

//CONSTANTS
#define CONFIGURACIO_TXSTA 0b00100000
#define CONFIGURACIO_RCSTA 0b10010000
#define CONFIGURACIO_BAUDC 0b00000000
#define TRUE            1 // TRUE
#define FALSE           0 // FALSE

#define MAX_LENGTH_CUA 80

//FUNCIONS
void SIO_Init();

// TX:
void SIO_SendCharCua(char str);
void SIO_SendString(char *str, unsigned char length);
void SIO_MotorTX();

// RX:
unsigned char SIO_LastByteReceived();
char SIO_GetCharCua();
void SIO_PseudoMotorRX();
unsigned char SIO_NewStringAvail();

#endif