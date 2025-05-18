#include "TJoystick.h"
#include "TADC.h"

/* =======================================
 *         PRIVATE CONSTANTS
 * ======================================= */

#define THRESHOLD_LOW 100
#define THRESHOLD_HIGH 180

#define AXIS_LOW 0
#define AXIS_NEUTRAL 1
#define AXIS_HIGH 2

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

BYTE currentDirection = JOY_CENTER;

/* =======================================
 *        PRIVATE FUNCTION HEADERS
 * ======================================= */

BYTE classify_axis(BYTE value);
BYTE direction_from_axes(BYTE x, BYTE y);
BYTE get_updated_direction(void);
BYTE get_next_direction(BYTE xDirection, BYTE yDirection);

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void Joystick_Init(void)
{
    TRISBbits.TRISB2 = 1; // Configure RB2 as digital input for button
}

BYTE Joystick_GetDirection(void)
{
    BYTE xAnalog = ADC_GetJoyX();
    BYTE yAnalog = ADC_GetJoyY();

    BYTE xDirection = classify_axis(xAnalog);
    BYTE yDirection = classify_axis(yAnalog);

    return get_next_direction(xDirection, yDirection);
}

BOOL Joystick_IsButtonPressed(void)
{
    return (PORTBbits.RB2 == 0) ? TRUE : FALSE;
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

BYTE classify_axis(BYTE value)
{
    if (value < THRESHOLD_LOW)
        return AXIS_LOW;
    if (value > THRESHOLD_HIGH)
        return AXIS_HIGH;
    return AXIS_NEUTRAL;
}

BYTE direction_from_axes(BYTE x, BYTE y)
{
    if (x == AXIS_HIGH)
        return JOY_RIGHT;
    if (x == AXIS_LOW)
        return JOY_LEFT;
    if (y == AXIS_HIGH)
        return JOY_DOWN;
    if (y == AXIS_LOW)
        return JOY_UP;
    return JOY_CENTER;
}

BYTE get_next_direction(BYTE xDirection, BYTE yDirection)
{
    BYTE previousDirection = currentDirection;
    currentDirection = direction_from_axes(xDirection, yDirection);

    if (previousDirection == currentDirection)
    {
        return JOY_CENTER;
    }

    return currentDirection;
}
