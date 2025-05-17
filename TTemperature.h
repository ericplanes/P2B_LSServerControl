#ifndef TTEMPERATURE_H
#define TTEMPERATURE_H

#include "Utils.h"

// Public function
SYS_STATUS TEMP_GetState(void);
BYTE TEMP_GetTemperature(void);
void TEMP_SimulateState(SYS_STATUS state);

#endif
