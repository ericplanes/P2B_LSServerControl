#include "TEEPROM.h"

#define LOG_SIZE 14
#define MAX_LOGS 15

static unsigned char mem_section = 0;
static unsigned char amount_of_stored_logs = 0;
static unsigned char pos = 0;

/* =======================================
 *          PRIVATE FUNCTION HEADERS
 * ======================================= */
static unsigned char read_byte(unsigned char address);
static void prepare_write_info(unsigned char address, unsigned char data);
static void write_prepared_info(void);
static void write_byte(unsigned char address, unsigned char data);

/* =======================================
 *          PUBLIC FUNCTION BODIES
 * ======================================= */

/**
 * Stores a new log entry into EEPROM progressively.
 * Returns EEPROM_FINISHED when the log is fully written.
 */
unsigned char EEPROM_store_log(const char *log_data)
{
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
            amount_of_stored_logs = MAX_LOGS;
        }

        write_byte(0, amount_of_stored_logs); // increase amount of stored logs
        return EEPROM_FINISHED;
    }

    return EEPROM_ONGOING;
}

/**
 * Reads a log entry from EEPROM progressively.
 * Returns EEPROM_FINISHED when the read is complete.
 */
unsigned char EEPROM_read_log(unsigned char section, char *log_data)
{
    if (pos < LOG_SIZE)
    {
        log_data[pos] = read_byte(pos + (section * LOG_SIZE) + 1);
        pos++;
    }

    if (pos >= LOG_SIZE)
    {
        pos = 0;
        return EEPROM_FINISHED;
    }

    return EEPROM_ONGOING;
}

/* =======================================
 *          PRIVATE FUNCTION BODIES
 * ======================================= */

static unsigned char read_byte(unsigned char address)
{
    EEADR = address;
    EECON1 = 0x01;
    return EEDATA;
}

static void prepare_write_info(unsigned char address, unsigned char data)
{
    EECON1bits.WREN = 1; // Enable write
    EEADR = address;     // Set addresss
    EEDATA = data;       // Set byte to be written
}

static void write_prepared_info(void)
{
    EECON2 = 0x55;       // Part 1/2 of write sequence
    EECON2 = 0xAA;       // Part 2/2 of write sequence
    EECON1bits.WR = 1;   // Write
    EECON1bits.WREN = 0; // Disable write
}

static void write_byte(unsigned char address, unsigned char data)
{
    prepare_write_info(address, data);
    di(); // Disable global interrupts
    write_prepared_info();
    ei(); // Re-enable global interrupts
}
