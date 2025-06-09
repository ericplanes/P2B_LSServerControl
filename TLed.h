#ifndef TLED_H
#define TLED_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

#define LED_OFF 0
#define LED_RED 1
#define LED_GREEN 2
#define LED_BLUE 3
#define LED_MAGENTA 4

void LED_Init(void);
void LED_SetColor(BYTE color);

#endif
