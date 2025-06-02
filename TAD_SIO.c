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
static char tx_buffer[MAX_LENGTH_CUA] = {0};
static char rx_buffer[MAX_LENGTH_CUA] = {0};

void consume_EOC(void);
void SIO_PrintString(const char *text);
void SIO_SafePrint(char lletra);

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
    if (PIR1bits.RC1IF)
    {
        rx_buffer[rx_head] = RCREG;
        rx_head++;
        if (rx_head >= MAX_LENGTH_CUA)
            rx_head = 0;
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
    char character = rx_buffer[rx_tail];

    // Clear the character from the buffer
    rx_buffer[rx_tail] = '\0';
    rx_tail++;

    if (rx_tail >= MAX_LENGTH_CUA)
    {
        rx_tail = 0;
    }

    return character;
}

unsigned char SIO_LastByteReceived(void)
{
    return (rx_head == 0) ? rx_buffer[MAX_LENGTH_CUA - 1] : rx_buffer[rx_head - 1];
}

void SIO_SendCharCua(char character)
{
    // Store the character in the buffer at the current head position
    tx_buffer[tx_head] = character;

    // Move to the next position in the buffer
    tx_head = tx_head + 1;

    // If we reach the end of the buffer, wrap around to the beginning
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

unsigned char SIO_GetCommandAndValue(unsigned char *value)
{
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
        consume_EOC();
        return command;
    case COMMAND_GET_GRAPH:
        consume_EOC();
        return command;
    case COMMAND_RESET:
        consume_EOC();
        return command;
    default:
        return NO_COMMAND;
    }

    // If INIT or SET TIME then read the rest of the code
    for (BYTE i = 0; i < len; i++)
        value[i] = SIO_GetCharCua();

    consume_EOC();
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

/* =======================================
 *        PRIVATE FUNCTION BODIES
 * ======================================= */

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
