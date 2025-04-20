#ifndef TSERIAL_H
#define TSERIAL_H

#include "Utils.h"

void Serial_Init(void);
BOOL Serial_CanReadChar(void);
BOOL Serial_CanSendChar(void);
BYTE Serial_ReadChar(void);
void Serial_SendChar(BYTE value);
void Serial_DebugPrintString(const BYTE *text);

#endif
