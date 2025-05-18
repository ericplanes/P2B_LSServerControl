#ifndef TTEMPERATURE_H
#define TTEMPERATURE_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

// Public function
SYS_STATUS TEMP_GetState(void);
BYTE TEMP_GetTemperature(void);

// Testing
void TEMP_TEST_SimulateState(SYS_STATUS state);

#endif
