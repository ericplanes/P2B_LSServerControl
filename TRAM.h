#ifndef TAD_RAM_H
#define TAD_RAM_H

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