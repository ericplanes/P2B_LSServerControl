#ifndef TJOYSTICK_H
#define TJOYSTICK_H

#include "Utils.h"

#define JOY_CENTER 'C'
#define JOY_UP 'U'
#define JOY_DOWN 'D'
#define JOY_LEFT 'L'
#define JOY_RIGHT 'R'
#define JOY_PRESSED 'S'

void Joystick_Init(void);
BYTE Joystick_GetDirection(void);

#endif
