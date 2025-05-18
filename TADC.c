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

static BYTE joyX = 0;
static BYTE joyY = 0;
static WORD temperature = 0;

static BYTE channel = 0;     // Current channel being read
static BYTE motor_state = 0; // FSM state

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

    ADCON1 = 0x0D;       // Configure AN0–AN2 as analog, rest as digital
    ADCON2 = 0b00111110; // Right justified result, acquisition time = 2 TAD
    ADCON0 = 0x00;       // Start with channel AN0
    ADCON0bits.ADON = 1; // Enable ADC module    channel = 0;

    motor_state = 0;
}

/**
 * Cooperative motor for ADC
 */
void ADC_Motor(void)
{
    switch (motor_state)
    {
    case 0: // Select and start conversion
        ADCON0bits.CHS = channel;
        ADCON0bits.GO_DONE = 1;
        motor_state = 1;
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
                temperature = (WORD)((ADRESH << 8) | ADRESL); // XXXXXXHH-LLLLLLLL
                channel = CHANNEL_X;
                break;
            }
            motor_state = 0;
        }
        break;
    }
}

/**
 * Returns latest ADC value for Joystick X (AN0).
 */
BYTE ADC_GetJoyX(void)
{
    return joyX;
}

/**
 * Returns latest ADC value for Joystick Y (AN1).
 */
BYTE ADC_GetJoyY(void)
{
    return joyY;
}

/**
 * Returns latest ADC value for temperature sensor TMP36 (AN2).
 */
WORD ADC_GetTemp(void)
{
    return temperature;
}
