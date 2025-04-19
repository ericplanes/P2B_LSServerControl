#ifndef _TTIMER_H_
#define _TTIMER_H_

#include <xc.h>
#include <pic18f4321.h>

#define TI_NUMTIMERS 10 // Amount of timers for this TAD, can be modified.
#define NO_TIMERS_AVAILABLE -1

void TiInit(void);
void Timer0_ISR(void);
char TiGetTimer(void);
unsigned int TiGetTics(unsigned char timerId);
void TiResetTics(unsigned char timerId);

#endif
