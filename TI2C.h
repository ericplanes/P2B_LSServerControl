#ifndef _TAD_I2C_H
#define _TAD_I2C_H

#include <xc.h>
#include <pic18f4321.h>
#include "TAD_SIO.h"
#include "Utils.h"

#define BAUDRATE 0x24 // Modify it to make I2C work at 100KHz

//-------------- Private functions: --------------
void I2C_Ready();
void I2C_Ack();
void I2C_Nack();
void fillTimestampLog(BYTE *log, BYTE seg, BYTE min, BYTE hora, BYTE dia, BYTE mes, BYTE anio);

//-------------- Public functions: --------------
void InitI2C();
char StartI2C(char);
char ReStartI2C_(char);
char I2C_Stop_(void);
char I2C_Write(unsigned char data);
char I2C_Read(char flag);
void DS3231_InitAlarm1_EverySecond(void);
void DS3231_LimpiarFlagAlarma1(void);
void DS3231_LeerFechaHora(unsigned char *seg, unsigned char *min, unsigned char *hora, unsigned char *dia_semana, unsigned char *dia, unsigned char *mes, unsigned char *anio);
void DS3231_SetFechaHora(unsigned char seg, unsigned char min, unsigned char hora, unsigned char dia_semana, unsigned char dia, unsigned char mes, unsigned char anio);

// Added functions
void I2C_ReadTimestamp(BYTE *hhmmssDDMMYYYY);
void testLectura(void);

#endif
