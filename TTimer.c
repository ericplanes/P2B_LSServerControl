#include "TTimer.h"

#define TI_MAXTICS 30000

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
static inline void TMR0_set_interruption_1ms(void)
{
    TMR0H = 0xF6;
    TMR0L = 0x36;
}

// Internal timer structure
typedef struct
{
    WORD startTics;
    BOOL inUse;
} Timer;

static Timer timers[TI_NUMTIMERS];
static WORD globalTics = 0;

/* =======================================
 *         INTERRUPT SERVICE ROUTINE
 * ======================================= */

void Timer0_ISR(void)
{
    INTCONbits.TMR0IF = 0;

    TMR0_set_interruption_1ms();
    globalTics++;

    if (globalTics >= TI_MAXTICS)
    {
        for (BYTE index = 0; index < TI_NUMTIMERS; index++)
        {
            if (timers[index].inUse == TRUE)
                timers[index].startTics -= globalTics;
        }
        globalTics = 0;
    }
}

/* =======================================
 *           PUBLIC FUNCTION BODIES
 * ======================================= */

void TiInit(void)
{
    for (BYTE index = 0; index < TI_NUMTIMERS; index++)
    {
        timers[index].inUse = FALSE;
    }

    globalTics = 0;

    T0CONbits.T08BIT = 0;
    T0CONbits.T0CS = 0;
    T0CONbits.PSA = 1;

    TMR0_set_interruption_1ms();
    T0CONbits.TMR0ON = 1;

    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1;
}

BYTE TiGetTimer(void)
{
    for (BYTE index = 0; index < TI_NUMTIMERS; index++)
    {
        if (timers[index].inUse == FALSE)
        {
            timers[index].inUse = TRUE;
            return index;
        }
    }
    return NO_TIMERS_AVAILABLE;
}

void TiResetTics(BYTE timerId)
{
    timers[timerId].startTics = globalTics;
}

WORD TiGetTics(BYTE timerId)
{
    return (globalTics - timers[timerId].startTics);
}
