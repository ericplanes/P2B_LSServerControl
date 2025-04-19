#include "TJoystick.h"
#include "TADC.h"

/* =======================================
 *         PRIVATE CONSTANTS
 * ======================================= */

#define THRESHOLD_LOW 100
#define THRESHOLD_HIGH 180

#define LOW 0
#define NEUTRAL 1
#define HIGH 2

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE currentDirection = JOY_CENTER;

/* =======================================
 *        PRIVATE FUNCTION HEADERS
 * ======================================= */

static BYTE classifyAxis(BYTE value);
static BYTE directionFromAxes(BYTE x, BYTE y);

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void Joystick_Init(void)
{
    // Configure RB2 as digital entry
    TRISBbits.TRISB2 = 1;
}

BYTE Joystick_GetDirection(void)
{
    // Get last values read form ADC
    BYTE xAnalog = ADC_GetJoyX();
    BYTE yAnalog = ADC_GetJoyY();

    // Compute the direction of each axis
    BYTE xDirection = classifyAxis(xAnalog);
    BYTE yDirection = classifyAxis(yAnalog);

    // Decide the joystick direction with prio to axis X
    currentDirection = directionFromAxes(xDirection, yDirection);
    return currentDirection;
}

BYTE Joystick_IsButtonPressed(void)
{
    if (PORTBbits.RB2 == 0)
    {
        return TRUE;
    }
    return FALSE;
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE classifyAxis(BYTE value)
{
    if (value < THRESHOLD_LOW)
    {
        return LOW;
    }
    if (value > THRESHOLD_HIGH)
    {
        return HIGH;
    }
    return NEUTRAL;
}

static BYTE directionFromAxes(BYTE x, BYTE y)
{
    if (x == HIGH)
    {
        return JOY_RIGHT;
    }
    if (x == LOW)
    {
        return JOY_LEFT;
    }
    if (y == HIGH)
    {
        return JOY_UP;
    }
    if (y == LOW)
    {
        return JOY_DOWN;
    }
    return JOY_CENTER;
}
