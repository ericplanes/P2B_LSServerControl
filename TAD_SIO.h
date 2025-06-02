#ifndef _TAD_SIO_H_
#define _TAD_SIO_H_

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

/* =======================================
 *              CONSTANTS
 * ======================================= */

#define MAX_LENGTH_CUA 80
#define NO_COMMAND 0

// Java → PIC commands
#define COMMAND_INITIALIZE 'N'
#define COMMAND_SET_TIME 'I'
#define COMMAND_GET_LOGS 'G'
#define COMMAND_GET_GRAPH 'E'
#define COMMAND_RESET 'X'

// PIC → Java commands
#define COMMAND_FINISH 'F'
#define COMMAND_DATAGRAPH 'A'
#define COMMAND_DATALOGS 'C'
#define COMMAND_UPDATETIME 'T'
#define COMMAND_UP 'U'
#define COMMAND_DOWN 'D'
#define COMMAND_LEFT 'L'
#define COMMAND_RIGHT 'R'
#define COMMAND_SELECT 'S'

// End-of-command characters
#define EOC1 '\r'
#define EOC2 '\n'

// Command lengths
#define LENGTH_INITIALIZE sizeof("Nyyyy-MM-dd HH:mm$RR$LT$MT$HT$CT")
#define LENGTH_SET_TIME sizeof("IHH:mm")

/* =======================================
 *         PUBLIC FUNCTION HEADERS
 * ======================================= */

void SIO_Init(void);

// TX
void SIO_SendCharCua(char c);
void SIO_SendString(char *str, unsigned char length);
void SIO_MotorTX(void);

// RX
void SIO_PseudoMotorRX(void);
char SIO_GetCharCua(void);
unsigned char SIO_LastByteReceived(void);

// Parsing
unsigned char SIO_GetCommandAndValue(unsigned char *value);
void SIO_parse_Initialize(unsigned char *value, BYTE *hour, BYTE *min, BYTE *day, BYTE *month, BYTE *year, BYTE *pollingRate, BYTE *lowThreshold, BYTE *moderateThreshold, BYTE *highThreshold);
void SIO_parse_SetTime(unsigned char *value, BYTE *hour, BYTE *min);

#endif