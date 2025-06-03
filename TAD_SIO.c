#include "TAD_SIO.h"

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

static void consume_EOC(void);
static void printString(const BYTE *text);
static void safePrint(BYTE lletra);
static BYTE getCharQueue(void);
static BYTE getLastByteReveived(void);
static BOOL isCommandInBuffer(void);

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
        BYTE c = RCREG;
        rx_buffer[rx_head] = c;
        rx_head = (rx_head + 1) % MAX_LENGTH_CUA;
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
    tx_buffer[tx_head] = character;
    tx_head = (tx_head + 1) % MAX_LENGTH_CUA;
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
    *year = (value[2] - '0') * 10 + (value[3] - '0');
    *month = (value[5] - '0') * 10 + (value[6] - '0');
    *day = (value[8] - '0') * 10 + (value[9] - '0');
    *hour = (value[11] - '0') * 10 + (value[12] - '0');
    *min = (value[14] - '0') * 10 + (value[15] - '0');
    *pollingRate = (value[17] - '0') * 10 + (value[18] - '0');
    *lowThreshold = (value[20] - '0') * 10 + (value[21] - '0');
    *moderateThreshold = (value[23] - '0') * 10 + (value[24] - '0');
    *highThreshold = (value[26] - '0') * 10 + (value[27] - '0');
}

void SIO_parse_SetTime(BYTE *value, BYTE *hour, BYTE *min)
{
    *hour = (value[0] - '0') * 10 + (value[1] - '0');
    *min = (value[3] - '0') * 10 + (value[4] - '0');
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
