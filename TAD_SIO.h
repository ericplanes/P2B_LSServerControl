#ifndef _SIO_H_
#define _SIO_H_

#include <xc.h>
#include <pic18f4321.h>

//CONSTANTS
#define CONFIGURACIO_TXSTA 0b00100000
#define CONFIGURACIO_RCSTA 0b10010000
#define CONFIGURACIO_BAUDC 0b00000000

// Length of the commands sent from java to the pic excluding the EOC characters
#define LENGTH_INITIALIZE   sizeof("Nyyyy-MM-dd HH:mm$RR$LT$MT$HT$CT") //  "yyyy-MM-ddHH:mm$pollingRate$lowThreshold$moderateThreshold$highThreshold$criticalThreshold\r\n" 
#define LENGTH_SET_TIME     sizeof("IHH:mm") // HH:mm format
#define LENGTH_GET_LOGS     1 // Just the command character 'G'
#define LENGTH_GET_GRAPH    1 // Just the command character 'E'
#define LENGTH_RESET        1 // Just the command character 'X'

// Commands sent from java to the pic
#define COMMAND_INITIALIZE  'N'
#define COMMAND_SET_TIME    'I'
#define COMMAND_GET_LOGS    'G'
#define COMMAND_GET_GRAPH   'E'
#define COMMAND_RESET       'X'

// Commands sent from pic to java
#define COMMAND_FINISH      'F'
#define COMMAND_DATAGRAPH   'A'
#define COMMAND_DATALOGS    'C'
#define COMMAND_UPDATETIME  'T'
#define COMMAND_UP          'U'
#define COMMAND_DOWN        'D'
#define COMMAND_LEFT        'L'
#define COMMAND_RIGHT       'R'
#define COMMAND_SELECT      'S'


#define EOC '\n' // End of Command, sent from pic to java, the last character of the string
#define NO_COMMAND 0 // End of Command, sent from pic to java, the last character of the string
#define TRUE 1
#define FALSE 0

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


unsigned char SIO_GetCommandAndValue(unsigned char* value);
void SIO_parse_Initialize(unsigned char* value, unsigned char *hour, unsigned char *min, unsigned char *day, unsigned char *month, unsigned char *year, unsigned char *pollingRate, unsigned char *lowThreshold, unsigned char *moderateThreshold, unsigned char *highThreshold, unsigned char *criticalThreshold);
void SIO_parse_SetTime(unsigned char* value, unsigned char *hour, unsigned char *min);
unsigned char SIO_SendInt(unsigned char value);


// future deprecated functions
unsigned char SIO_RXAvail(void);
unsigned char SIO_TXAvail(void); // Available == 1
unsigned char SIO_GetChar(void);
void SIO_PutChar(unsigned char lletra);
void SIO_SafePrint(char lletra);
void SIO_PrintString(const char *text);
void itoa(unsigned int value, char *str, unsigned char base);

#endif