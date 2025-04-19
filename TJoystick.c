#include "TJoystick.h"
#include "TADC.h"
#include "Utils.h"

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

static unsigned char currentDirection = JOY_CENTER;

/* =======================================
 *        PRIVATE FUNCTION HEADERS
 * ======================================= */

static unsigned char classifyAxis(unsigned char value);
static unsigned char directionFromAxes(unsigned char x, unsigned char y);

/* =======================================
 *        PUBLIC FUNCTION BODIES
 * ======================================= */

void Joystick_Init(void)
{
    // Configure RB2 as digital entry
    TRISBbits.TRISB2 = 1;
}

unsigned char Joystick_GetDirection(void)
{
    // Get last values read form ADC
    unsigned char xAnalog = ADC_GetJoyX();
    unsigned char yAnalog = ADC_GetJoyY();

    // Compute the direction of each axis
    unsigned char xDirection = classifyAxis(xAnalog);
    unsigned char yDirection = classifyAxis(yAnalog);

    // Decide the joystick direction with prio to axis X
    currentDirection = directionFromAxes(xDirection, yDirection);
    return currentDirection;
}

unsigned char Joystick_IsButtonPressed(void)
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

static unsigned char classifyAxis(unsigned char value)
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

static unsigned char directionFromAxes(unsigned char x, unsigned char y)
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
