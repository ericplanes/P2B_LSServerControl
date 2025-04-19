#ifndef TMENU_H
#define TMENU_H

#include <stdint.h>
#include <xc.h>         // include processor files - each processor file is guarded.
#include <pic18f4321.h> // no necessari a MPLAB

// Initialization and motor
void Menu_Init(void);
void Menu_Motor(void);

// Configuration getters
const uint8_t *Menu_GetTMPThresholds(void);
uint8_t Menu_GetSamplingTime(void);
const char *Menu_GetInitialTimeString(void);

#endif
