#ifndef TTEMPERATURE_H
#define TTEMPERATURE_H

#include <stdint.h>

// Temperature states
#define TEMP_LOW 0
#define TEMP_MOD 1
#define TEMP_HIGH 2
#define TEMP_CRIT 3

// Public function
uint8_t Temperature_GetState(void);

#endif
