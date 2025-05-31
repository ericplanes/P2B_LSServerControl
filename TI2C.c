#include "TI2C.h"

#define DS3231_ADDRESS (0x68 << 1)

/* =======================================
 *         PRIVATE FUNCTION HEADERS
 * ======================================= */

static void i2c_ready(void);
static void i2c_ack(void);
static void i2c_nack(void);
static char i2c_start(char address);
static char i2c_restart(char address);
static char i2c_write(BYTE data);
static char i2c_stop(void);
static char i2c_read(char ack);
static BYTE bin_to_bcd(BYTE val);
static BYTE bcd_to_bin(BYTE val);
static void ds3231_read_raw(BYTE *h, BYTE *m, BYTE *s, BYTE *dow, BYTE *d, BYTE *mo, BYTE *y);
static void ds3231_write_raw(BYTE h, BYTE m, BYTE s, BYTE dow, BYTE d, BYTE mo, BYTE y);
static void ds3231_update_raw(BYTE h, BYTE m);
static void ds3231_clear_alarm_flag(void);
static void fill_timestamp(BYTE *log, BYTE sec, BYTE min, BYTE hour, BYTE day, BYTE month, BYTE year);

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void I2C_Init(void)
{
  TRISCbits.TRISC3 = 1;
  TRISCbits.TRISC4 = 1;
  TRISBbits.RB1 = 1;

  INTCON2bits.RBPU = 0;
  SSPSTAT = 0x80;
  SSPCON1 = 0x28;
  SSPCON2 = 0x00;
  SSPADD = I2C_BAUDRATE;
  PIE1bits.SSPIE = 1;
  PIR1bits.SSPIF = 0;
}

void I2C_ReadTimestamp(BYTE *timestamp)
{
  BYTE s, m, h, dow, d, mo, y;
  ds3231_read_raw(&h, &m, &s, &dow, &d, &mo, &y);
  fill_timestamp(timestamp, s, m, h, d, mo, y);
}

void I2C_SetTimestamp(BYTE hour, BYTE min, BYTE sec, BYTE weekday, BYTE day, BYTE month, BYTE year)
{
  ds3231_write_raw(hour, min, sec, weekday, day, month, year);
}

void I2C_TEST_PrintTimestamp(void)
{
  BYTE s, m, h, dow, d, mo, y;
  char buffer[6];

  if (!PORTBbits.RB1)
    ds3231_clear_alarm_flag();

  ds3231_read_raw(&h, &m, &s, &dow, &d, &mo, &y);

  SIO_PrintString("\r\n");
  itoa(d, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint('/');
  itoa(mo, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint('/');
  itoa(y, buffer, 10);
  SIO_PrintString(buffer);
  SIO_PrintString(" ");
  itoa(h, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint(':');
  itoa(m, buffer, 10);
  SIO_PrintString(buffer);
  SIO_SafePrint(':');
  itoa(s, buffer, 10);
  SIO_PrintString(buffer);
  SIO_PrintString("\r\n");
}

void I2C_TEST_Wait1S(void)
{
  while (PORTBbits.RB1)
    ; // Wait for alarm flag (falling edge on RB1)
  ds3231_clear_alarm_flag();
}

void I2C_TEST_InitAlarmEverySecond(void)
{
  // Set Alarm1 registers to trigger every second (A1Mx = 1)
  i2c_start(DS3231_ADDRESS);
  i2c_write(0x07); // Start at Alarm1 Seconds register
  i2c_write(0x80); // A1M1 = 1
  i2c_write(0x80); // A1M2 = 1
  i2c_write(0x80); // A1M3 = 1
  i2c_write(0x80); // A1M4 = 1
  i2c_stop();

  // Set control register: INTCN = 1, A1IE = 1
  i2c_start(DS3231_ADDRESS);
  i2c_write(0x0E);       // Control register
  i2c_write(0b00000101); // INTCN = 1, A1IE = 1
  i2c_stop();

  // Clear Alarm1 flag (A1F bit in status register 0x0F)
  ds3231_clear_alarm_flag();
}

void I2C_UpdateTimestamp(BYTE hour, BYTE min)
{
  ds3231_update_raw(hour, min);
}

/* =======================================
 *         PRIVATE FUNCTION BODIES
 * ======================================= */

static void i2c_ready(void)
{
  while (PIR2bits.BCLIF)
    ; // Wait if bit collision interrupt flag is set
  while (SSPSTATbits.BF || (SSPSTATbits.R_nW))
    ;                 //
  PIR1bits.SSPIF = 0; // Clear SSPIF interrupt flag
}

static void i2c_ack(void)
{
  SSPCON2bits.ACKDT = 0;
  SSPCON2bits.ACKEN = 1;
  while (SSPCON2bits.ACKEN)
    ;
}

static void i2c_nack(void)
{
  SSPCON2bits.ACKDT = 1;
  SSPCON2bits.ACKEN = 1;
  while (SSPCON2bits.ACKEN)
    ;
}

static char i2c_start(char address)
{
  SSPCON2bits.SEN = 1;
  while (SSPCON2bits.SEN)
    ;
  PIR1bits.SSPIF = 0;
  if (!(SSPSTATbits.S))
    return 0;
  return i2c_write(address);
}

static char i2c_restart(char address)
{
  SSPCON2bits.RSEN = 1;
  while (SSPCON2bits.RSEN)
    ;
  PIR1bits.SSPIF = 0;
  if (!(SSPSTATbits.S))
    return 0;
  return i2c_write(address);
}

static char i2c_write(BYTE data)
{
  SSPBUF = data; // Write data to SSPBUF
  i2c_ready();
  return (SSPCON2bits.ACKSTAT ? 1 : 2);
}

static char i2c_stop(void)
{
  i2c_ready();
  SSPCON2bits.PEN = 1; // Stop communication
  while (SSPCON2bits.PEN)
    ; // Wait for end of stop pulse
  PIR1bits.SSPIF = 0;
  return (!SSPSTATbits.P ? 0 : 1);
}

static char i2c_read(char ack)
{
  SSPCON2bits.RCEN = 1; // Enable receive
  while (!SSPSTATbits.BF)
    ;
  char buffer = SSPBUF;
  if (ack == 0)
    i2c_ack();
  else
    i2c_nack();
  i2c_ready();
  return buffer;
}

static void ds3231_clear_alarm_flag(void)
{
  i2c_start(DS3231_ADDRESS);
  i2c_write(0x0F);
  i2c_write(0x00);
  i2c_stop();
}

static BYTE bin_to_bcd(BYTE val)
{
  return (BYTE)((val / 10) << 4) | (val % 10);
}

static BYTE bcd_to_bin(BYTE val)
{
  return ((val >> 4) * 10) + (val & 0x0F);
}

static void ds3231_read_raw(BYTE *h, BYTE *m, BYTE *s, BYTE *dow, BYTE *d, BYTE *mo, BYTE *y)
{
  i2c_start(DS3231_ADDRESS);
  i2c_write(0x00);
  i2c_stop();

  i2c_start(DS3231_ADDRESS | 1);

  *s = bcd_to_bin(i2c_read(0) & 0x7F);
  *m = bcd_to_bin(i2c_read(0) & 0x7F);
  *h = bcd_to_bin(i2c_read(0) & 0x3F);
  *dow = bcd_to_bin(i2c_read(0) & 0x07);
  *d = bcd_to_bin(i2c_read(0) & 0x3F);
  *mo = bcd_to_bin(i2c_read(0) & 0x1F);
  *y = bcd_to_bin(i2c_read(1));

  i2c_stop();
}

static void ds3231_write_raw(BYTE h, BYTE m, BYTE s, BYTE dow, BYTE d, BYTE mo, BYTE y)
{
  i2c_start(DS3231_ADDRESS);
  i2c_write(0x00);
  i2c_write(bin_to_bcd(s & 0x7F));
  i2c_write(bin_to_bcd(m & 0x7F));
  i2c_write(bin_to_bcd(h & 0x3F));
  i2c_write(bin_to_bcd(dow & 0x07));
  i2c_write(bin_to_bcd(d & 0x3F));
  i2c_write(bin_to_bcd(mo & 0x1F));
  i2c_write(bin_to_bcd(y));
  i2c_stop(); // End I2C comunication
}

/*
 * The goal is to only update minutes and hour from the I2C.
 */
static void ds3231_update_raw(BYTE h, BYTE m)
{
  i2c_start(DS3231_ADDRESS);
  i2c_write(bin_to_bcd(m & 0x7F));
  i2c_write(bin_to_bcd(h & 0x3F));
  i2c_stop(); // End I2C comunication
}

static void fill_timestamp(BYTE *log, BYTE sec, BYTE min, BYTE hour, BYTE day, BYTE month, BYTE year)
{
  log[0] = '0' + (hour / 10);
  log[1] = '0' + (hour % 10);
  log[2] = '0' + (min / 10);
  log[3] = '0' + (min % 10);
  log[4] = '0' + (sec / 10);
  log[5] = '0' + (sec % 10);
  log[6] = '0' + (day / 10);
  log[7] = '0' + (day % 10);
  log[8] = '0' + (month / 10);
  log[9] = '0' + (month % 10);
  log[10] = '2'; // Year 20xx
  log[11] = '0';
  log[12] = '0' + (year / 10);
  log[13] = '0' + (year % 10);
}
