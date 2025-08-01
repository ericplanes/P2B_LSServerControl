#include "TEEPROM.h"

#define LOG_SIZE TIMESTAMP_SIZE - 1
#define MAX_LOGS 15

#define EEPROM_IDLE 0
#define EEPROM_WRITING 1
#define EEPROM_READING 2

#define ADDR_JUMP 3
#define ADDR_STORED_LOGS 0x00
#define ADDR_MEM_SECTION 0x01

static BYTE mem_section = 0;
static BYTE amount_of_stored_logs = 0;
static BYTE write_pos = 0; // Separate position counter for writing
static BYTE read_pos = 0;  // Separate position counter for reading
static BYTE eeprom_state = EEPROM_IDLE;

/* =======================================
 *       PRIVATE FUNCTION HEADERS
 * ======================================= */
static BYTE read_byte(BYTE address);
static BYTE read_stored_logs_count(void);
static BYTE read_mem_section_safe(void);
static void prepare_write_info(BYTE address, BYTE data);
static void write_prepared_info(void);
static void write_byte(BYTE address, BYTE data);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

void EEPROM_Init(void)
{
    amount_of_stored_logs = read_stored_logs_count();
    mem_section = read_mem_section_safe();
    write_pos = 0;
    read_pos = 0;
    eeprom_state = EEPROM_IDLE;
}

void EEPROM_CleanMemory(void)
{
    // Reset state variables
    mem_section = 0x00;
    amount_of_stored_logs = 0x00;
    write_pos = 0;
    read_pos = 0;
    eeprom_state = EEPROM_IDLE;

    // Total EEPROM space used:
    BYTE total_bytes = (MAX_LOGS * LOG_SIZE) + ADDR_JUMP;

    // Clean all used EEPROM bytes
    for (BYTE addr = 0; addr < total_bytes; addr++)
    {
        write_byte(addr, 0x00);
    }

    // Optionally store reset header
    write_byte(ADDR_STORED_LOGS, 0x00);
    write_byte(ADDR_MEM_SECTION, 0x00);
}

BOOL EEPROM_StoreLog(const BYTE *log_data)
{
    if (eeprom_state == EEPROM_READING || EECON1bits.WR)
        return FALSE;

    eeprom_state = EEPROM_WRITING;

    if (write_pos < LOG_SIZE)
    {
        write_byte(write_pos + (mem_section * LOG_SIZE) + ADDR_JUMP, log_data[write_pos]);
        write_pos++;
    }

    if (write_pos == LOG_SIZE)
    {
        write_pos = 0;
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

    if (read_pos < LOG_SIZE)
    {
        log_data[read_pos] = read_byte(read_pos + (section * LOG_SIZE) + ADDR_JUMP);
        read_pos++;
    }

    if (read_pos == LOG_SIZE)
    {
        log_data[read_pos] = '\0';
        eeprom_state = EEPROM_IDLE;
        read_pos = 0;
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
    return (previous_section + 1) % MAX_LOGS;
}

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

static BYTE read_stored_logs_count(void)
{
    BYTE count = read_byte(ADDR_STORED_LOGS);

    // Check if corrupted (uninitialized EEPROM or invalid value)
    if (count > MAX_LOGS)
    {
        count = 0;                       // Return safe default
        write_byte(ADDR_STORED_LOGS, 0); // Correct the EEPROM
    }

    return count;
}

static BYTE read_mem_section_safe(void)
{
    BYTE section = read_byte(ADDR_MEM_SECTION);

    // Check if corrupted (uninitialized EEPROM or invalid value)
    if (section >= MAX_LOGS || section == 0xFF)
    {
        section = 0;                     // Return safe default
        write_byte(ADDR_MEM_SECTION, 0); // Correct the EEPROM
    }

    return section;
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
        ;                // Wait for WR to become 0 (end of write operation)
    PIR2bits.EEIF = 0;   // Clear the write flag
    EECON1bits.WREN = 0; // Disable write
}

static void write_byte(BYTE address, BYTE data)
{
    prepare_write_info(address, data);
    di();
    write_prepared_info();
    ei();
}
