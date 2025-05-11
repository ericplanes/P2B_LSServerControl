#ifndef TRAM_H
#define TRAM_H

#include <xc.h>
#include <pic18f4321.h>

#include "Utils.h"
#include "TAD_SIO.h"

// Stores a temperature value into RAM progressively (simulated)
BOOL RAM_StoreTemperature(WORD temperature);
void RAM_Init(void);
void RAM_Write(unsigned char data);
unsigned char RAM_Read(void);
void RAM_DebugPrint(void);
void RAM_Reset(void);
void count(void);
void count_reset(void);
void RAM_WriteIncremental(void);
void RAM_ReadAndPrint100(void);
#endif