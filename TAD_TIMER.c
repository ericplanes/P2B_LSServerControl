#include "TAD_TIMER.h"

#define TMR0_INT_2MS 63536 // 2 ms con Fosc = 32 MHz y prescaler 1:8
#define TI_NUMTIMERS 4	   // Amount of timers being used on the system
#define T0CON_CONFIG 0b10000010
// TMR0 ON | 16-bit | Prescaler 1:8 | Internal Clock
// Bit 7: TMR0ON = 1
// Bit 6: T08BIT = 0 (16-bit)
// Bit 5: T0CS = 0 (Internal clock)
// Bit 4: T0SE = 0 (irrelevant for internal clock)
// Bit 3: PSA = 0 (prescaler assigned)
// Bits 2-0: T0PS = 010 → 1:8 prescaler

struct Timer
{
	WORD TicsInicials;
	BYTE Busy;
} static Timers[TI_NUMTIMERS];

static volatile WORD Tics = 0;

void Timer0_ISR()
{
	TMR0 = TMR0_INT_2MS;
	TMR0IF = 0;
	Tics++;
}

void TiInit()
{
	for (BYTE counter = 0; counter < TI_NUMTIMERS; counter++)
	{
		Timers[counter].Busy = FALSE;
	}
	T0CON = T0CON_CONFIG;
	TMR0 = TMR0_INT_2MS;
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1;
}

BYTE TiNewTimer(BYTE *TimerHandle)
{
	BYTE Comptador = 0;
	while (Timers[Comptador].Busy == TRUE)
	{
		if (++Comptador == TI_NUMTIMERS)
			return (FALSE);
	}
	Timers[Comptador].Busy = TRUE;
	*TimerHandle = Comptador;
	return (TRUE);
}

void TiResetTics(BYTE TimerHandle)
{
	di();
	Timers[TimerHandle].TicsInicials = Tics;
	ei();
}

WORD TiGetTics(BYTE TimerHandle)
{
	di();
	WORD CopiaTicsActual = Tics;
	ei();
	return (CopiaTicsActual - (Timers[TimerHandle].TicsInicials));
}
