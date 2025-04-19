#include "TADC.h"

/* =======================================
 *              CONSTANTS
 * ======================================= */

#define CHANNEL_X 0    // AN0
#define CHANNEL_Y 1    // AN1
#define CHANNEL_TEMP 2 // AN2

/* =======================================
 *          PRIVATE VARIABLES
 * ======================================= */

static unsigned char joyX = 0;
static unsigned char joyY = 0;
static unsigned char temperature = 0;

static unsigned char channel = 0; // Current channel being read
static unsigned char state = 0;   // FSM state

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Initializes ADC hardware and configures AN0, AN1, AN2.
 */
void ADC_Init(void)
{
    TRISAbits.TRISA0 = 1; // AN0 (Joystick X)
    TRISAbits.TRISA1 = 1; // AN1 (Joystick Y)
    TRISAbits.TRISA2 = 1; // AN2 (TMP36)

    ADCON1 = 0x0D; // Configure AN0–AN2 as analog, rest as digital
    ADCON2 = 0x09; // Right justified result, acquisition time = 2 TAD
    ADCON0 = 0x00; // Start with channel AN0

    channel = 0;
    state = 0;
}

/**
 * Cooperative motor for ADC
 */
void ADC_Motor(void)
{
    switch (state)
    {
    case 0: // Select and start conversion
        ADCON0bits.CHS = channel;
        ADCON0bits.GO_DONE = 1;
        state = 1;
        break;

    case 1: // Wait until conversion completes
        if (ADCON0bits.GO_DONE == 0)
        {
            switch (channel)
            {
            case CHANNEL_X:
                joyX = ADRESH;
                channel = CHANNEL_Y;
                break;

            case CHANNEL_Y:
                joyY = ADRESH;
                channel = CHANNEL_TEMP;
                break;

            case CHANNEL_TEMP:
                temperature = ADRESH;
                channel = CHANNEL_X;
                break;
            }
            state = 0;
        }
        break;
    }
}

/**
 * Returns latest ADC value for Joystick X (AN0).
 */
unsigned char ADC_GetJoyX(void)
{
    return joyX;
}

/**
 * Returns latest ADC value for Joystick Y (AN1).
 */
unsigned char ADC_GetJoyY(void)
{
    return joyY;
}

/**
 * Returns latest ADC value for temperature sensor TMP36 (AN2).
 */
unsigned char ADC_GetTemp(void)
{
    return temperature;
}
