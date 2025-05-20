// josepmaria.ribes@salle.url.edu (si hi trobeu alguna errada, si us plau envieu-me un correu :-)
// Arbeca, bressol de l'oliva arbequina
// Mar�, any del Senyor de 2023

// TAD TIMER. Honor i gl�ria


#include <xc.h>
#include "pic18f4321.h"
#include "TAD_TIMER.h"

// Definicions, per interrupci� cada 2ms.
#define T0CON_CONFIG 0x82
#define RECARREGA_TMR0 64911        // 2 ms, suposant FOsc a 10MHz.

#define TI_NUMTIMERS 4              // Nombre de timers virtuals gestionats per aquest TAD. Si cal, s'incrementa o es disminueix...

// VARIABLES GLOBALS DEL TAD
struct Timer {
	WORD TicsInicials;
	BYTE Busy;
} static Timers[TI_NUMTIMERS];

static volatile WORD Tics=0;

void Timer0_ISR () {
    // Pre: IMPORTANT! Funci� que ha der ser cridada des de la RSI, en en cas que TMR0IF==1.
    TMR0=RECARREGA_TMR0;
    TMR0IF=0;
    Tics++;    
}

void TiInit () {
	for (BYTE counter=0; counter<TI_NUMTIMERS; counter++) {
		Timers[counter].Busy=TI_FALS;
	}
	T0CON=T0CON_CONFIG;
    TMR0=RECARREGA_TMR0;
	INTCONbits.TMR0IF = 0;
	INTCONbits.TMR0IE = 1;
    // Caldr� que des del main o des d'on sigui s'activin les interrupcions globals!
}

BYTE TiNewTimer(BYTE *TimerHandle) {
	BYTE Comptador=0;
	while (Timers[Comptador].Busy==TI_CERT) {
		if (++Comptador == TI_NUMTIMERS) return (TI_FALS);
	}
	Timers[Comptador].Busy=TI_CERT;
	*TimerHandle=Comptador;
    return (TI_CERT);
}

void TiResetTics (BYTE TimerHandle) {
	di(); Timers[TimerHandle].TicsInicials=Tics; ei();
}


WORD TiGetTics (BYTE TimerHandle) {
    di(); WORD CopiaTicsActual=Tics; ei();
	return (CopiaTicsActual-(Timers[TimerHandle].TicsInicials));
}

void TI_CloseTimer (BYTE TimerHandle) {
	Timers[TimerHandle].Busy=TI_FALS;
}

void TI_End () {
}