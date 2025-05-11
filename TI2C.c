#include "TI2C.h"

#define DS3231_ADDRESS 0x68 << 1 // Dirección I2C del DS3231 (esclavo)

//-------------- Private functions: --------------

void I2C_Ready()
{
  while (PIR2bits.BCLIF)
    ; /* Wait if bit collision interrupt flag is set*/
  while (SSPSTATbits.BF || (SSPSTATbits.R_nW))
    ;
  PIR1bits.SSPIF = 0; /* Clear SSPIF interrupt flag*/
}

void I2C_Ack()
{
  SSPCON2bits.ACKDT = 0; /* Acknowledge data 1:NACK,0:ACK */
  SSPCON2bits.ACKEN = 1; /* Enable ACK/NACK to send */
  while (SSPCON2bits.ACKEN)
    ;
}

void I2C_Nack()
{
  SSPCON2bits.ACKDT = 1; /* Acknowledge data 1:NACK,0:ACK */
  SSPCON2bits.ACKEN = 1; /* Enable ACK/NACK to send */
  while (SSPCON2bits.ACKEN)
    ;
}

//-------------- Public functions: --------------

void InitI2C()
{
  TRISCbits.TRISC3 = 1;
  TRISCbits.TRISC4 = 1;
  TRISBbits.RB1 = 1;
  INTCON2bits.RBPU = 0;
  SSPSTAT = 0x80;
  SSPCON1 = 0x28;
  SSPCON2 = 0x0;
  SSPADD = BAUDRATE;
  PIE1bits.SSPIE = 1;
  PIR1bits.SSPIF = 0;
}

char StartI2C(char slave_rw_addr)
{
  SSPCON2bits.SEN = 1;
  while (SSPCON2bits.SEN)
    ;
  PIR1bits.SSPIF = 0;
  if (!(SSPSTATbits.S))
  {
    return 0;
  }
  return (I2C_Write(slave_rw_addr));
}

char ReStartI2C_(char slave_rw_addr)
{
  SSPCON2bits.RSEN = 1;
  while (SSPCON2bits.RSEN)
    ;
  PIR1bits.SSPIF = 0;
  if (!(SSPSTATbits.S))
  {
    return 0;
  }
  return (I2C_Write(slave_rw_addr));
}

char I2C_Write(unsigned char data)
{
  SSPBUF = data; /* Write data to SSPBUF*/
  I2C_Ready();
  if (SSPCON2bits.ACKSTAT) /* Check for acknowledge bit from slave*/
    return 1;
  else
    return 2;
}

char I2C_Stop_()
{
  I2C_Ready();
  SSPCON2bits.PEN = 1; /* Stop communication*/
  while (SSPCON2bits.PEN)
    ; /* Wait for end of stop pulse*/
  PIR1bits.SSPIF = 0;
  if (!SSPSTATbits.P)
    return 0;
  return 1;
}

char I2C_Read(char flag)
{
  char buffer = 0;
  SSPCON2bits.RCEN = 1; /* Enable receive */

  /* Wait for buffer full flag which when complete byte received */
  while (!SSPSTATbits.BF)
    ;
  buffer = SSPBUF; /* Copy SSPBUF to buffer */

  /* Send acknowledgment or negative acknowledgment after read to
  continue or stop reading */
  if (flag == 0)
    I2C_Ack();
  else
    I2C_Nack();
  I2C_Ready();
  return (buffer);
}

// Inicializa la alarma 1 para que se active cada segundo
void DS3231_InitAlarm1_EverySecond()
{
  // Apuntar al registro 0x07 (Alarm1 Seconds)
  StartI2C(DS3231_ADDRESS);

  I2C_Write(0x07); // Dirección del primer registro de alarma 1

  // Configurar los registros de alarma con los bits A1Mx = 1 para "cada segundo"
  I2C_Write(0x80); // Alarm1 Seconds: A1M1 = 1
  I2C_Write(0x80); // Alarm1 Minutes: A1M2 = 1
  I2C_Write(0x80); // Alarm1 Hours:   A1M3 = 1
  I2C_Write(0x80); // Alarm1 Day/Date: A1M4 = 1, DY/DT = 0

  I2C_Stop_();

  // Configurar el registro de control (0x0E): INTCN = 1, A1IE = 1
  StartI2C(DS3231_ADDRESS);
  I2C_Write(0x0E);
  I2C_Write(0b00000101); // INTCN=1, A1IE=1, otras en 0
  I2C_Stop_();

  // Limpiar el flag de la alarma 1 (A1F en 0x0F)
  DS3231_LimpiarFlagAlarma1();
}

void DS3231_LimpiarFlagAlarma1()
{
  StartI2C(DS3231_ADDRESS);
  I2C_Write(0x0F); // Dirección del registro de estado
  I2C_Write(0x00); // Escribimos 0 para borrar el flag A1F
  I2C_Stop_();
}

unsigned char BINtoBCD(unsigned char val)
{
  return ((val / 10) << 4) | (val % 10);
}

void DS3231_SetFechaHora(unsigned char seg, unsigned char min, unsigned char hora,
                         unsigned char dia_semana, unsigned char dia,
                         unsigned char mes, unsigned char anio)
{
  StartI2C(DS3231_ADDRESS); // Iniciar comunicación I2C con DS3231
  I2C_Write(0x00);          // Apuntar al registro de segundos

  I2C_Write(BINtoBCD(seg & 0x7F));        // 7 bits para segundos (bit 7 = 0)
  I2C_Write(BINtoBCD(min & 0x7F));        // 7 bits para minutos
  I2C_Write(BINtoBCD(hora & 0x3F));       // 6 bits para horas en formato 24h
  I2C_Write(BINtoBCD(dia_semana & 0x07)); // 3 bits válidos para día semana
  I2C_Write(BINtoBCD(dia & 0x3F));        // 6 bits para día del mes
  I2C_Write(BINtoBCD(mes & 0x1F));        // 5 bits para mes
  I2C_Write(BINtoBCD(anio));              // Año en dos dígitos (00-99)

  I2C_Stop_(); // Finalizar comunicación I2C
}

unsigned char BCDtoBIN(unsigned char val)
{
  return ((val >> 4) * 10) + (val & 0x0F);
}

void DS3231_LeerFechaHora(unsigned char *seg, unsigned char *min, unsigned char *hora,
                          unsigned char *dia_semana, unsigned char *dia,
                          unsigned char *mes, unsigned char *anio)
{
  StartI2C(DS3231_ADDRESS);
  I2C_Write(0x00); // Apuntar al registro de segundos
  I2C_Stop_();

  StartI2C(DS3231_ADDRESS | 1); // Lectura (LSB = 1)

  *seg = BCDtoBIN(I2C_Read(0) & 0x7F);        // bits 6:0
  *min = BCDtoBIN(I2C_Read(0) & 0x7F);        // bits 6:0
  *hora = BCDtoBIN(I2C_Read(0) & 0x3F);       // 24h, bits 5:0
  *dia_semana = BCDtoBIN(I2C_Read(0) & 0x07); // bits 2:0
  *dia = BCDtoBIN(I2C_Read(0) & 0x3F);        // bits 5:0
  *mes = BCDtoBIN(I2C_Read(0) & 0x1F);        // bits 4:0
  *anio = BCDtoBIN(I2C_Read(1));              // última lectura con NACK

  I2C_Stop_();
}

void testLectura(void)
{
  // Inicializar el DS3231

  InitI2C();
  DS3231_InitAlarm1_EverySecond();

  unsigned char seg, min, hora, dia_sem, dia, mes, anio;
  char buffer[6];

  if (!PORTBbits.RB1) // condicio de la alarma
    DS3231_LimpiarFlagAlarma1();

  DS3231_LeerFechaHora(&seg, &min, &hora, &dia_sem, &dia, &mes, &anio);
  SIO_PrintString("\r\n");
  // Imprimir fecha en formato DD/MM/AA
  itoa(dia, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint('/');
  itoa(mes, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint('/');
  itoa(anio, buffer, 10);
  SIO_PrintString(buffer);
  SIO_PrintString(" ");

  // Imprimir hora en formato HH:MM:SS
  itoa(hora, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint(':');
  itoa(min, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint(':');
  itoa(seg, buffer, 10);
  SIO_PrintString(buffer);
  SIO_PrintString("\r\n");
}