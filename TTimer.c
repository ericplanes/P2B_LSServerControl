#include "TTimer.h"

#define TI_FALSE 0
#define TI_TRUE 1

#define TI_MAXTICS 30000 // Max time span to measure (30-40 seconds)

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
static void TMR0_set_interruption_1ms();

// Internal timer structure
typedef struct
{
    unsigned int startTics;
    unsigned char inUse;
} Timer;

static Timer timers[TI_NUMTIMERS];
static unsigned int globalTics = 0;
static int i;

/* =======================================
 *         INTERRUPT SERVICE ROUTINE
 * ======================================= */

void Timer0_ISR(void)
{
    INTCONbits.TMR0IF = 0; // Clear Timer0 interrupt flag

    // Reinitialize Timer0 for the next interrupt (1ms)
    TMR0_set_interruption_1ms();
    globalTics++;

    // Prevent overflow by normalizing all timers if max tics is reached
    if (globalTics >= TI_MAXTICS)
    {
        for (i = 0; i < TI_NUMTIMERS; i++)
        {
            if (timers[i].inUse == TI_TRUE)
                timers[i].startTics -= globalTics;
        }
        globalTics = 0;
    }
}

/* =======================================
 *           PUBLIC FUNCTION BODIES
 * ======================================= */

void TiInit(void)
{
    for (i = 0; i < TI_NUMTIMERS; i++)
    {
        timers[i].inUse = TI_FALSE;
    }

    globalTics = 0;

    // Timer0 configuration
    T0CONbits.T08BIT = 0; // 16-bit mode
    T0CONbits.T0CS = 0;   // Clock = Fosc/4
    T0CONbits.PSA = 1;    // No prescaler

    TMR0_set_interruption_1ms();
    T0CONbits.TMR0ON = 1; // Enable Timer0

    // Interrupt configuration
    INTCONbits.GIE = 1;    // Global interrupt enable
    INTCONbits.PEIE = 1;   // Peripheral Interrupt Enable (in case we want to use it for RTC)
    INTCONbits.TMR0IF = 0; // Overflow not reached, 1 when reached
    INTCONbits.TMR0IE = 1; // Timer0 interrupt enable
}

char TiGetTimer(void)
{
    for (char timerId = 0; timerId < TI_NUMTIMERS; timerId++)
    {
        if (timers[timerId].inUse == TI_FALSE)
        {
            timers[timerId].inUse = TI_TRUE;
            return timerId;
        }
    }
    return NO_TIMERS_AVAILABLE;
}

void TiResetTics(unsigned char timerId)
{
    timers[timerId].startTics = globalTics;
}

unsigned int TiGetTics(unsigned char timerId)
{
    return (globalTics - timers[timerId].startTics);
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

static void TMR0_set_interruption_1ms()
{
    TMR0H = 216;
    TMR0L = 240;
}