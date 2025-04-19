#ifndef TMENU_H
#define TMENU_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

// Initialization and motor
void Menu_Init(void);
void Menu_Motor(void);

// Configuration getters
const WORD *Menu_GetTMPThresholds(void);
BYTE Menu_GetSamplingTime(void);
const BYTE *Menu_GetInitialTimeString(void);

#endif
