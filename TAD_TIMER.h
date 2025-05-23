#ifndef T_TIMER_H
#define T_TIMER_H

#include "Utils.h"

#include <xc.h>
#include "pic18f4321.h"

#define ONE_SECOND 500 // 1 interruption every 2ms

#define TI_TEST 0
#define TI_FAN 1
#define TI_LED 2
#define TI_CONT 3

void Timer0_ISR(void);

void TiInit(void);
// Post: Constructor. It is a global precondition to have called this function before calling any other function of the TAD.

BYTE TiNewTimer(BYTE *TimerHandle);
// Post: Returns TI_CERT if a new timer was successfully created, and TI_FALSE otherwise.
// Sets *TimerHandle to the assigned timer identifier, which is needed to use TI_GetTics and TI_ResetTics.

void TiResetTics(BYTE TimerHandle);
// Pre: Handle has been returned by TiNewTimer.
// Post: Starts the timing associated with 'TimerHandle', storing the time reference at the moment of the call.

WORD TiGetTics(BYTE TimerHandle);
// Pre: Handle has been returned by TiNewTimer.
// Post: Returns the number of ticks elapsed since the call to TI_ResetTics for the same TimerHandle.

#endif
