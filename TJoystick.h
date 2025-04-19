#ifndef TJOYSTICK_H
#define TJOYSTICK_H

#include <stdint.h>

#define JOY_CENTER 0
#define JOY_UP 1
#define JOY_DOWN 2
#define JOY_LEFT 3
#define JOY_RIGHT 4

void Joystick_Init(void);
unsigned char Joystick_GetDirection(void);
unsigned char Joystick_IsButtonPressed(void);

#endif
