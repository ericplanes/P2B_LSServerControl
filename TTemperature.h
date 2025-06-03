#ifndef TTEMPERATURE_H
#define TTEMPERATURE_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"
#include "TMenu.h"
#include "TADC.h"

// Public function
void TEMP_Init(void);
SYS_STATUS TEMP_GetState(void);
BYTE TEMP_GetTemperature(void);

#endif
