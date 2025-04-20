#ifndef TFAN_H
#define TFAN_H

#include "Utils.h"

#define FAN_OFF 0
#define FAN_ON 1

void FAN_Init(void);
void FAN_SetStateA(BYTE state);
void FAN_SetStateB(BYTE state);

#endif
