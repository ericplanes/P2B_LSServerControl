#ifndef TFAN_H
#define TFAN_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

void FAN_Init(void);
void FAN_SetPowerA(BOOL enabled);
void FAN_SetPowerB(BOOL enabled);

#endif
