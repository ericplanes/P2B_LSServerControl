#ifndef TRAM_H
#define TRAM_H

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"
#include "TAD_SIO.h"

void RAM_Init(void);
void RAM_Write(BYTE data);
BYTE RAM_Read(void);
void RAM_Reset(void);

#endif
