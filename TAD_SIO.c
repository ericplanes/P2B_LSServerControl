#include "TAD_SIO.h"
#include <string.h>

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
static char tx_buffer[MAX_LENGTH_CUA] = {0};

static BYTE rx_index = 0;
static BYTE rx_read_index = 0;
static BOOL rx_ready = FALSE;
static char rx_buffer[MAX_LENGTH_CUA] = {0};

void consume_EOC(void);
void SIO_PrintString(const char *text);
void SIO_SafePrint(char lletra);
static void reset_rx_buffer(void);

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
    rx_index = rx_read_index = 0;
    rx_ready = FALSE;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}

void SIO_PseudoMotorRX(void)
{
    if (rx_ready)
        return; // already received a command

    if (PIR1bits.RC1IF)
    {
        char c = RCREG;
        if (rx_index < MAX_LENGTH_CUA - 1)
        {
            rx_buffer[rx_index++] = c;
            if (c == '\n')
            {
                rx_buffer[rx_index] = '\0';
                rx_ready = TRUE;
            }
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
            tx_tail++;
            if (tx_tail >= MAX_LENGTH_CUA)
                tx_tail = 0;
            tx_state = STATE_TX_WAIT;
        }
        break;

    case STATE_TX_WAIT:
        tx_state = (tx_tail == tx_head) ? STATE_TX_IDLE : STATE_TX_SEND;
        break;
    }
}

char SIO_GetCharCua(void)
{
    return rx_buffer[rx_read_index++];
}

unsigned char SIO_GetCommandAndValue(unsigned char *value)
{
    if (!rx_ready)
        return NO_COMMAND;

    rx_read_index = 0; // reset for reading
    BYTE command = SIO_GetCharCua();
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
        break;
    default:
        reset_rx_buffer();
        return NO_COMMAND;
    }

    for (BYTE i = 0; i < len; i++)
        value[i] = SIO_GetCharCua();

    consume_EOC();
    reset_rx_buffer();
    return command;
}

void SIO_parse_Initialize(unsigned char *value, BYTE *hour, BYTE *min, BYTE *day, BYTE *month, BYTE *year, BYTE *pollingRate, BYTE *lowThreshold, BYTE *moderateThreshold, BYTE *highThreshold)
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

void SIO_parse_SetTime(unsigned char *value, BYTE *hour, BYTE *min)
{
    *hour = (value[0] - '0') * 10 + (value[1] - '0');
    *min = (value[3] - '0') * 10 + (value[4] - '0');
}

void SIO_SendCharCua(char character)
{
    tx_buffer[tx_head] = character;
    tx_head = tx_head + 1;
    if (tx_head >= MAX_LENGTH_CUA)
    {
        tx_head = 0;
    }
}

void SIO_SendString(char *str, unsigned char length)
{
    for (unsigned char i = 0; i < length; i++)
        SIO_SendCharCua(str[i]);
}

void consume_EOC(void)
{
    SIO_GetCharCua(); // \r
    SIO_GetCharCua(); // \n
}

void SIO_PrintString(const char *text)
{
    while (*text != '\0')
    {
        SIO_SafePrint(*text++);
    }
}

void SIO_SafePrint(char lletra)
{
    if (PIR1bits.TXIF == 1)
        TXREG = lletra;
}

static void reset_rx_buffer(void)
{
    rx_index = 0;
    rx_read_index = 0;
    rx_ready = FALSE;
    memset(rx_buffer, 0, sizeof(rx_buffer));
}
