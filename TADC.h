#ifndef TADC_H
#define TADC_H

// Main
void ADC_Init(void);
void ADC_Motor(void);

// Joystick
unsigned char ADC_GetJoyX(void);
unsigned char ADC_GetJoyY(void);

// Temperature
unsigned char ADC_GetTemp(void);

#endif
