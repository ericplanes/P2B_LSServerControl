#include "TEEPROM.h"

#define LOG_SIZE (TIMESTAMP_SIZE) - 1
#define MAX_LOGS 15

#define EEPROM_IDLE 0
#define EEPROM_WRITING 1
#define EEPROM_READING 2

#define ADDR_STORED_LOGS 0
#define ADDR_MEM_SECTION 1

static BYTE mem_section = 0;
static BYTE amount_of_stored_logs = 0;
static BYTE pos = 0;
static BYTE eeprom_state = EEPROM_IDLE;

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE read_byte(BYTE address)
{
    EEADR = address;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.RD = 1;
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
    EECON1bits.EEPGD = 0; // Data EEPROM
    EECON1bits.CFGS = 0;  // Access EEPROM
    EECON1bits.WREN = 1;

    EECON2 = 0x55;
    EECON2 = 0xAA;
    EECON1bits.WR = 1; // Start write

    while (EECON1bits.WR)
        ;                // Espera que WR es posi a 0 (final de l'escriptura)
    PIR2bits.EEIF = 0;   // Neteja el flag d'escriptura
    EECON1bits.WREN = 0; // Desactiva escriptura
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

void EEPROM_Init(void)
{
    amount_of_stored_logs = read_byte(ADDR_STORED_LOGS);
    mem_section = read_byte(ADDR_MEM_SECTION);
    pos = 0;
    eeprom_state = EEPROM_IDLE;
}

void EEPROM_CleanMemory(void)
{
    // Reset state variables
    mem_section = 0;
    amount_of_stored_logs = 0;
    pos = 0;
    eeprom_state = EEPROM_IDLE;

    // Total EEPROM space used:
    BYTE total_bytes = (MAX_LOGS * LOG_SIZE) + 2;

    // Clean all used EEPROM bytes
    for (BYTE addr = 0; addr < total_bytes; addr++)
    {
        write_byte(addr, 0x00);
    }

    // Optionally store reset header
    write_byte(ADDR_STORED_LOGS, 0);
    write_byte(ADDR_MEM_SECTION, 0);
}

BOOL EEPROM_StoreLog(const BYTE *log_data)
{
    if (eeprom_state == EEPROM_READING || EECON1bits.WR)
        return FALSE;

    eeprom_state = EEPROM_WRITING;

    if (pos < LOG_SIZE)
    {
        write_byte(pos + (mem_section * LOG_SIZE) + 2, log_data[pos]);
        pos++;
    }

    if (pos == LOG_SIZE)
    {
        pos = 0;
        mem_section++;

        if (amount_of_stored_logs < MAX_LOGS)
            amount_of_stored_logs++;

        if (mem_section == MAX_LOGS)
        {
            mem_section = 0;
            amount_of_stored_logs = MAX_LOGS;
        }

        write_byte(ADDR_STORED_LOGS, amount_of_stored_logs);
        write_byte(ADDR_MEM_SECTION, mem_section);
        eeprom_state = EEPROM_IDLE;
        return TRUE;
    }

    return FALSE;
}

BOOL EEPROM_ReadLog(BYTE section, BYTE *log_data)
{
    if (eeprom_state == EEPROM_WRITING)
        return FALSE;

    eeprom_state = EEPROM_READING;

    if (pos < LOG_SIZE)
    {
        log_data[pos] = read_byte(pos + (section * LOG_SIZE) + 2);
        pos++;
    }

    if (pos == LOG_SIZE)
    {
        log_data[pos] = '\0';
        eeprom_state = EEPROM_IDLE;
        pos = 0;
        return TRUE;
    }

    return FALSE;
}

BYTE EEPROM_GetAmountOfStoredLogs(void)
{
    return amount_of_stored_logs;
}

BYTE EEPROM_GetFirstSection(void)
{
    return amount_of_stored_logs != MAX_LOGS ? 0 : mem_section;
}

BYTE EEPROM_GetNextSection(BYTE previous_section)
{
    return previous_section == MAX_LOGS ? 0 : previous_section + 1;
}
