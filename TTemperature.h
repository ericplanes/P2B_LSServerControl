#ifndef TTEMPERATURE_H
#define TTEMPERATURE_H

#include "Utils.h"

// Temperature states
#define TEMP_LOW 0
#define TEMP_MOD 1
#define TEMP_HIGH 2
#define TEMP_CRIT 3

// Public function
BYTE TEMP_GetState(void);

#endif
