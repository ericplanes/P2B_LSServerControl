#include "TTimer.h"

#define TI_MAXTICS 30000

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */

// Assume Fosc = 8 MHz ⇒ Tosc = 125 ns
// ⇒ Fosc/4 = 2 MHz ⇒ Ttick = 500 ns
// With 1:256 prescaler ⇒ Ttick = 128 μs
// For 1 ms overflow: 1 ms / 128 μs = 7.8125 ticks
// So: preload = 65536 - 8 = 65528 = 0xFFF8

static inline void TMR0_set_interruption_1ms(void)
{
    TMR0H = 0xFF;
    TMR0L = 0xF8;
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

    T0CONbits.T08BIT = 0;   // 16-bit timer
    T0CONbits.T0CS = 0;     // Internal instruction clock (Fosc/4)
    T0CONbits.PSA = 0;      // Prescaler assigned
    T0CONbits.T0PS = 0b111; // 1:256 prescaler

    TMR0_set_interruption_1ms(); // Load TMR0 for 1ms overflow

    T0CONbits.TMR0ON = 1; // Turn on Timer0

    INTCONbits.GIE = 1;    // Enable global interrupts
    INTCONbits.PEIE = 1;   // Enable peripheral interrupts
    INTCONbits.TMR0IF = 0; // Clear interrupt flag
    INTCONbits.TMR0IE = 1; // Enable Timer0 interrupt
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
