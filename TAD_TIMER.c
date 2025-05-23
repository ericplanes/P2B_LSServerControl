#include "TAD_TIMER.h"

#define T0CON_CONFIG 0x82
#define TMR0_INT_2MS 64911 // 2 ms, if FOsc is 10MHz.
#define TI_NUMTIMERS 4	   // Amount of timers being used on the system

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
