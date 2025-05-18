#ifndef TMENU_H
#define TMENU_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

// Initialization and motor
void MENU_Init(void);
void MENU_Motor(void);

// Configuration getters
BOOL MENU_isConfigured(void);
const BYTE *MENU_GetTMPThresholds(void);
BYTE MENU_GetSamplingTime(void);
const BYTE *MENU_GetInitialTimeString(void);

// Visible for testing
void MENU_TEST_SetDefaultConfig(void);

#endif
