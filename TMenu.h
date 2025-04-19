#ifndef TMENU_H
#define TMENU_H

#include <stdint.h>

// main
void Menu_Init(void);
void Menu_Motor(void);

const uint8_t *Menu_GetTMPThresholds(void);
uint8_t Menu_GetSamplingTime(void);

#endif
