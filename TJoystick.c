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

static BYTE currentDirection = JOY_CENTER;

/* =======================================
 *        PRIVATE FUNCTION HEADERS
 * ======================================= */

static BYTE classify_axis(BYTE value);
static BYTE direction_from_axes(BYTE x, BYTE y);

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

    currentDirection = direction_from_axes(xDirection, yDirection);
    return currentDirection;
}

BOOL Joystick_IsButtonPressed(void)
{
    return (PORTBbits.RB2 == 0) ? TRUE : FALSE;
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE classify_axis(BYTE value)
{
    if (value < THRESHOLD_LOW)
        return AXIS_LOW;
    if (value > THRESHOLD_HIGH)
        return AXIS_HIGH;
    return AXIS_NEUTRAL;
}

static BYTE direction_from_axes(BYTE x, BYTE y)
{
    if (x == AXIS_HIGH)
        return JOY_RIGHT;
    if (x == AXIS_LOW)
        return JOY_LEFT;
    if (y == AXIS_HIGH)
        return JOY_UP;
    if (y == AXIS_LOW)
        return JOY_DOWN;
    return JOY_CENTER;
}
