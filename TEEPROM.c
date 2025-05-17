#include "TEEPROM.h"

#define LOG_SIZE (TIMESTAMP_SIZE - 1)
#define MAX_LOGS 15

static BYTE mem_section = 0;
static BYTE amount_of_stored_logs = 0;
static BYTE pos = 0;

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE read_byte(BYTE address)
{
    EEADR = address;
    EECON1bits.EEPGD = 0; // Acceder a memoria de datos, no FLASH
    EECON1bits.CFGS = 0;  // Acceder a EEPROM, no configuración
    EECON1bits.RD = 1;    // Iniciar lectura
    return EEDATA;
}

static void prepare_write_info(BYTE address, BYTE data)
{
    EECON1bits.WREN = 1;
    EEADR = address;
    EEDATA = data;
}

static void write_prepared_info(void)
{
    EECON1bits.EEPGD = 0; // Acceso a EEPROM (no FLASH)
    EECON1bits.CFGS = 0;  // Acceso a EEPROM (no configuración)
    EECON1bits.WREN = 1;
    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1;
    while (EECON1bits.WR);
    EECON1bits.WREN = 0;
}

static void write_byte(BYTE address, BYTE data)
{
    prepare_write_info(address, data);
    di();
    write_prepared_info();
    ei();
}

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

BOOL EEPROM_StoreLog(const BYTE *log_data)
{
    if (EECON1bits.WR)  // EEPROM is busy writing, exit early
        return FALSE;

    if (pos < LOG_SIZE)
    {
        write_byte(pos + (mem_section * LOG_SIZE) + 1, log_data[pos]);
        pos++;
    }

    if (pos == LOG_SIZE)
    {
        pos = 0;
        mem_section++;
        amount_of_stored_logs++;

        if (mem_section == MAX_LOGS)
        {
            mem_section = 0;
            amount_of_stored_logs = MAX_LOGS; // cap the count
        }

        write_byte(0, amount_of_stored_logs);
        return TRUE;
    }

    return FALSE;
}

BOOL EEPROM_ReadLog(BYTE section, BYTE *log_data)
{
    if (pos < LOG_SIZE)
    {
        log_data[pos] = read_byte(pos + (section * LOG_SIZE) + 1);
        pos++;
    }

    if (pos == LOG_SIZE)
    {
        log_data[pos] = '\0';
        pos++;
    }

    if (pos > LOG_SIZE)
    {
        pos = 0;
        return TRUE;
    }

    return FALSE;
}
