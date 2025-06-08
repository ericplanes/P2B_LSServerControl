#include "TAD_SIO.h"
#include "TLed.h"

/* =======================================
 *           PRIVATE DEFINES
 * ======================================= */

#define STATE_TX_IDLE 0
#define STATE_TX_SEND 1
#define STATE_TX_WAIT 2

/* =======================================
 *         PRIVATE VARIABLES
 * ======================================= */

static BYTE tx_state = STATE_TX_IDLE;
static BYTE tx_head = 0, tx_tail = 0;
static BYTE rx_head = 0, rx_tail = 0;
static BYTE tx_buffer[MAX_LENGTH_CUA] = {0};
static BYTE rx_buffer[MAX_LENGTH_CUA] = {0};

// Error tracking variables
static BYTE overrun_error_count = 0;
static BYTE framing_error_count = 0;
static BYTE buffer_overflow_count = 0;
static BYTE tx_timeout_count = 0;

static void consume_EOC(void);
static void printString(const BYTE *text);
static void safePrint(BYTE lletra);
static BYTE getCharQueue(void);
static BYTE getLastByteReveived(void);
static BOOL isCommandInBuffer(void);
static BOOL isValidCommand(BYTE command);
static void clearRXBuffer(void);
static BOOL isValidDigit(BYTE c);
static BYTE safeParseDigitPair(BYTE *value, BYTE index);

/* =======================================
 *         PUBLIC FUNCTION BODIES
 * ======================================= */

void SIO_Init(void)
{
    TRISCbits.TRISC6 = 0; // TX output
    TRISCbits.TRISC7 = 1; // RX input

    TXSTAbits.BRGH = 1;
    BAUDCONbits.BRG16 = 0;
    SPBRG = 207; // 9600 baud @ 32 MHz

    TXSTAbits.SYNC = 0;
    TXSTAbits.TXEN = 1;
    RCSTAbits.SPEN = 1;
    RCSTAbits.CREN = 1;

    tx_state = STATE_TX_IDLE;
    tx_head = tx_tail = 0;
    rx_head = rx_tail = 0;
}

void SIO_PseudoMotorRX(void)
{
    if (getLastByteReveived() == '\n' && rx_tail != rx_head)
        return;

    if (PIR1bits.RC1IF)
    {
        // Check for UART hardware errors first
        if (RCSTAbits.OERR) // Overrun error
        {
            // Clear overrun error by resetting receiver
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
            // Flush any corrupt data
            while (PIR1bits.RC1IF)
            {
                volatile BYTE dummy = RCREG;
            }
            LED_SetColor(LED_RED); // Visual indication of error
            overrun_error_count++;
            return;
        }

        if (RCSTAbits.FERR) // Framing error
        {
            // Clear framing error by reading RCREG
            volatile BYTE dummy = RCREG;
            LED_SetColor(LED_GREEN); // Visual indication of framing error
            framing_error_count++;
            return;
        }

        // Read data only if no errors
        BYTE c = RCREG;

        // Check for buffer overflow before storing
        BYTE next_head = (rx_head + 1) % MAX_LENGTH_CUA;
        if (next_head == rx_tail)
        {
            // Buffer overflow - discard oldest data to make room
            rx_tail = (rx_tail + 1) % MAX_LENGTH_CUA;
            LED_SetColor(LED_MAGENTA); // Visual indication of buffer overflow
            buffer_overflow_count++;
        }

        rx_buffer[rx_head] = c;
        rx_head = next_head;

        if (c == COMMAND_RESET)
        {
            LED_SetColor(LED_WHITE);
            while (TRUE)
                ;
        }
    }
}

void SIO_MotorTX(void)
{
    switch (tx_state)
    {
    case STATE_TX_IDLE:
        if (tx_tail != tx_head)
            tx_state = STATE_TX_SEND;
        break;

    case STATE_TX_SEND:
        if (PIR1bits.TXIF)
        {
            TXREG = tx_buffer[tx_tail];
            tx_buffer[tx_tail] = '\0';
            tx_tail = (tx_tail + 1) % MAX_LENGTH_CUA;
            tx_state = STATE_TX_WAIT;
        }
        break;

    case STATE_TX_WAIT:
        tx_state = (tx_tail == tx_head) ? STATE_TX_IDLE : STATE_TX_SEND;
        break;
    }
}

void SIO_SendCharCua(BYTE character)
{
    BYTE next = (tx_head + 1) % MAX_LENGTH_CUA;
    if (next != tx_tail) // Evita sobreescriure dades no llegides
    {
        tx_buffer[tx_head] = character;
        tx_head = next;
    }
}

void SIO_SendString(BYTE *str, BYTE length)
{
    for (BYTE i = 0; i < length; i++)
        SIO_SendCharCua(str[i]);
}

BYTE SIO_GetCommandAndValue(BYTE *value)
{
    if (!isCommandInBuffer())
        return NO_COMMAND;

    BYTE command = getCharQueue();
    BYTE len = 0;

    LED_Toggle();

    switch (command)
    {
    case COMMAND_INITIALIZE:
        len = LENGTH_INITIALIZE - 1;
        break;
    case COMMAND_SET_TIME:
        len = LENGTH_SET_TIME - 1;
        break;
    case COMMAND_GET_LOGS:
    case COMMAND_GET_GRAPH:
    case COMMAND_RESET:
        consume_EOC();
        return command;
    default:
        return NO_COMMAND;
    }

    for (BYTE i = 0; i < len; i++)
        value[i] = getCharQueue();

    consume_EOC();
    return command;
}

void SIO_parse_Initialize(BYTE *value, BYTE *hour, BYTE *min, BYTE *day, BYTE *month, BYTE *year, BYTE *pollingRate, BYTE *lowThreshold, BYTE *moderateThreshold, BYTE *highThreshold)
{
    // Validate command length first
    BYTE len = 0;
    while (value[len] != '\0' && len < 35)
        len++;
    if (len < 28) // Minimum required length
    {
        // Set safe defaults for invalid input
        *year = 24;
        *month = 1;
        *day = 1;
        *hour = 0;
        *min = 0;
        *pollingRate = 10;
        *lowThreshold = 20;
        *moderateThreshold = 30;
        *highThreshold = 40;
        return;
    }

    *year = safeParseDigitPair(value, 2);
    *month = safeParseDigitPair(value, 5);
    *day = safeParseDigitPair(value, 8);
    *hour = safeParseDigitPair(value, 11);
    *min = safeParseDigitPair(value, 14);
    *pollingRate = safeParseDigitPair(value, 17);
    *lowThreshold = safeParseDigitPair(value, 20);
    *moderateThreshold = safeParseDigitPair(value, 23);
    *highThreshold = safeParseDigitPair(value, 26);

    // Validate ranges
    if (*month == 0 || *month > 12)
        *month = 1;
    if (*day == 0 || *day > 31)
        *day = 1;
    if (*hour > 23)
        *hour = 0;
    if (*min > 59)
        *min = 0;
    if (*pollingRate == 0 || *pollingRate > 60)
        *pollingRate = 10;

    // Ensure thresholds are in ascending order
    if (*lowThreshold >= *moderateThreshold)
        *moderateThreshold = *lowThreshold + 5;
    if (*moderateThreshold >= *highThreshold)
        *highThreshold = *moderateThreshold + 5;
}

void SIO_parse_SetTime(BYTE *value, BYTE *hour, BYTE *min)
{
    // Validate command length first
    BYTE len = 0;
    while (value[len] != '\0' && len < 35)
        len++;
    if (len < 5) // Minimum required length "HH:MM"
    {
        *hour = 0;
        *min = 0;
        return;
    }

    *hour = safeParseDigitPair(value, 0);
    *min = safeParseDigitPair(value, 3);

    // Validate ranges
    if (*hour > 23)
        *hour = 0;
    if (*min > 59)
        *min = 0;
}

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

static BYTE getCharQueue(void)
{
    BYTE character = rx_buffer[rx_tail];
    rx_tail = (rx_tail + 1) % MAX_LENGTH_CUA;
    return character;
}

static BYTE getLastByteReveived(void)
{
    return (rx_head == 0) ? rx_buffer[MAX_LENGTH_CUA - 1] : rx_buffer[(rx_head - 1) % MAX_LENGTH_CUA];
}

static void consume_EOC(void)
{
    while (getCharQueue() != '\n')
        ;
}

static BOOL isCommandInBuffer(void)
{
    BYTE i = rx_tail;
    while (i != rx_head)
    {
        if (rx_buffer[i] == '\n')
            return TRUE;

        i = (i + 1) % MAX_LENGTH_CUA;
    }
    return FALSE;
}

static void printString(const BYTE *text)
{
    while (*text != '\0')
    {
        safePrint(*text++);
    }
}

static void safePrint(BYTE lletra)
{
    if (PIR1bits.TXIF == 1)
        TXREG = lletra;
}

static BOOL isValidCommand(BYTE command)
{
    return (command == COMMAND_INITIALIZE ||
            command == COMMAND_SET_TIME ||
            command == COMMAND_GET_LOGS ||
            command == COMMAND_GET_GRAPH ||
            command == COMMAND_RESET);
}

static void clearRXBuffer(void)
{
    rx_head = rx_tail = 0;
    for (BYTE i = 0; i < MAX_LENGTH_CUA; i++)
        rx_buffer[i] = 0;
}

static BOOL isValidDigit(BYTE c)
{
    return (c >= '0' && c <= '9');
}

static BYTE safeParseDigitPair(BYTE *value, BYTE index)
{
    if (!isValidDigit(value[index]) || !isValidDigit(value[index + 1]))
        return 0; // Return 0 for invalid input
    return (value[index] - '0') * 10 + (value[index + 1] - '0');
}
