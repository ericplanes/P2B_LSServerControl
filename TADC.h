#ifndef TADC_H
#define TADC_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

// Main
void ADC_Init(void);
void ADC_Motor(void);

// Joystick
BYTE ADC_GetJoyX(void);
BYTE ADC_GetJoyY(void);

// Temperature
WORD ADC_GetTemp(void);

#endif
