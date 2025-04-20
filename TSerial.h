#ifndef TSERIAL_H
#define TSERIAL_H

#include "Utils.h"

void Serial_Init(void);
BOOL Serial_CanReadChar(void);
BYTE Serial_ReadChar(void);
BOOL Serial_CanSendChar(void);
void Serial_SendChar(BYTE value);

#endif
