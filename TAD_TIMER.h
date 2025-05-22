#ifndef T_TIMER_H
#define T_TIMER_H

#include "Utils.h"

#define ONE_SECOND 500 // 1 interruption each 2ms

#define TI_TEST 0
#define TI_FAN 1
#define TI_LED 2
#define TI_CONT 3

void Timer0_ISR(void);

void TiInit(void);
// Post: Constructor. �s precondici� global haver cridat aquesta funci� quan es cridi qualsevol altra funci� del TAD

BYTE TiNewTimer(BYTE *TimerHandle);
// Post: Retorna TI_CERT en cas que s'hagi creat adequadament un nou timer, i TI_FALS en cas contrati.
// Posa a *TimerHandle l'identificador de timer assignat, necessari per usar les funcions TI_GetTics i TI_ResetTics.

void TiResetTics(BYTE TimerHandle);
// Pre: Handle ha estat retornat per Ti_NewTimer.
// Post: Engega la temporitzaci� associada a 'TimerHandle', guardant la referencia temporal en el moment de la crida.

WORD TiGetTics(BYTE TimerHandle);
// Pre: Handle ha estat retornat per TI_NewGetTimer.
// Post: Retorna els tics transcorreguts des de la crida a TI_ResetTics per al mateix TimerHandle.

#endif
