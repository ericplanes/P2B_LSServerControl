#include "TAD_SIO.h"

#define E_IDLE_ENDED        0
#define E_RX_1              1
#define E_TX_1              2
#define E_TX_2              5
#define E_SET_LLINDARS_TEMP 3
#define E_SET_T_MOSTREIG    4

unsigned char EstatSIOTx,
              EstatSIORx,
              PtrRedTx,
              PtrGreenTx,
              PtrRedRx,
              PtrGreenRx;
char CuaRX[MAX_LENGTH_CUA] = {0};
char CuaTX[MAX_LENGTH_CUA] = {0};

void SIO_Init(){
    /*
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    
    TXSTA = CONFIGURACIO_TXSTA;
    RCSTA = CONFIGURACIO_RCSTA;
    BAUDCON = CONFIGURACIO_BAUDC;
    SPBRG = 64;
   

    EstatSIOTx = E_IDLE_ENDED;
    PtrRedTx = 0;
    PtrGreenTx = 0;
    PtrRedRx = 0;
    PtrGreenRx = 0;
    */
    TRISCbits.TRISC6 = 0;
    TRISCbits.TRISC7 = 1;

    TXSTAbits.BRGH = 1;    // High Baud Rate Select bit (para mayor precisio?n)
    BAUDCONbits.BRG16 = 0; // Usar el Baud Rate Generator de 8 bits
    SPBRG = 64;            // Para 9600 baudios con Fosc = 10 MHz

    TXSTAbits.SYNC = 0; // Modo asi?ncrono
    TXSTAbits.TXEN = 1; // Habilitar transmisio?n
    RCSTAbits.SPEN = 1; // Habilitar puerto serie
    RCSTAbits.CREN = 1; // Habilitar recepció
}

// SIO RX:
unsigned char SIO_RXAvail(){
    return ((PIR1bits.RCIF==1)?TRUE:FALSE);
}

char SIO_GetCharCua(){
    char caractero = CuaRX[PtrGreenRx];
    CuaRX[PtrGreenRx++] = '\0';
    if(PtrGreenRx >= MAX_LENGTH_CUA) PtrGreenRx = 0;
    return caractero;
}

unsigned char SIO_NewStringAvail(){
    return (PtrGreenRx != PtrRedRx);
}

unsigned char SIO_LastByteReceived(){
    return CuaRX[PtrRedRx-1];
}

void SIO_PseudoMotorRX(){
    if(PIR1bits.RC1IF == 1){
        CuaRX[PtrRedRx++] = RCREG;
        if(PtrRedRx >= MAX_LENGTH_CUA) PtrRedRx = 0;
    }
}

unsigned char SIO_CheckCuaRx(){
    // Retorna 1 si hi ha algo a llegir
    return (PtrGreenRx != PtrRedRx);
}

// SIO TX:
void SIO_SendString(char *str, unsigned char length){ 
    for(unsigned char i = 0; i < length; i++){
        CuaTX[PtrGreenTx++] = str[i];
        if(PtrGreenTx >= MAX_LENGTH_CUA) PtrGreenTx = 0;
    }
}

// test function, might be better than SIO_SendString
void SIO_SendStringUntilEnd(char *str){
    while(*str){
        //write
        CuaTX[PtrGreenTx++] = *str++;
    }
}

void SIO_SendCharCua(char str){
    CuaTX[PtrGreenTx++] = str;
    if(PtrGreenTx >= MAX_LENGTH_CUA) PtrGreenTx = 0;
}

void SIO_MotorTX(){
    switch(EstatSIOTx){
        case 0:
            if(PtrGreenTx != PtrRedTx){
                EstatSIOTx = 1;
            }
        break;
        case 1:
            if(PIR1bits.TXIF==1){
                TXREG = CuaTX[PtrRedTx];
                CuaTX[PtrRedTx++] = '\0';
                if(PtrRedTx >= MAX_LENGTH_CUA) PtrRedTx = 0;
                EstatSIOTx = 2;
            }
        break;
        case 2:
            if(PtrGreenTx == PtrRedTx){
                EstatSIOTx = 0;
            }else{
                EstatSIOTx = 1;
            }
        break;
    }
}

/* =======================================
 *       Funcions per parsejar java
 * ======================================= */

unsigned char SIO_GetCommandAndValue(unsigned char* value){
    if(SIO_LastByteReceived() != '\n') return NO_COMMAND; // No hi ha cap comanda
    switch(SIO_GetCharCua()){
        case COMMAND_INITIALIZE:
            for(unsigned char i = 0; i < LENGTH_INITIALIZE - 1; i++){
                value[i] = SIO_GetCharCua();
            }
            SIO_GetCharCua();
            SIO_GetCharCua(); // Llegeix els dos caracters de final de comanda
            return COMMAND_INITIALIZE;
            break;
        case COMMAND_SET_TIME:
            for(unsigned char i = 0; i < LENGTH_SET_TIME - 1; i++){
                value[i] = SIO_GetCharCua();
            }
            SIO_GetCharCua();
            SIO_GetCharCua(); // Llegeix els dos caracters de final de comanda
            return COMMAND_SET_TIME;
            break;
        case COMMAND_GET_LOGS:
            return COMMAND_GET_LOGS;
        case COMMAND_GET_GRAPH:
            return COMMAND_GET_GRAPH;
        case COMMAND_RESET:
            return COMMAND_RESET;
        default:
            return NO_COMMAND; // Comanda desconeguda
    }
}

void parse_Initialize(unsigned char* value, unsigned char *hour, unsigned char *min, unsigned char *day, unsigned char *month, unsigned char *year, unsigned char *pollingRate, unsigned char *lowThreshold, unsigned char *moderateThreshold, unsigned char *highThreshold, unsigned char *criticalThreshold)
{
    // Format: "yyyy-MM-dd HH:mm$PR$LT$MT$HT$CT"
    *year = (value[2] - '0') * 10 + (value[3] - '0');     // Solo los dos últimos dígitos
    *month = (value[5] - '0') * 10 + (value[6] - '0');
    *day = (value[8] - '0') * 10 + (value[9] - '0');
    *hour = (value[11] - '0') * 10 + (value[12] - '0');
    *min = (value[14] - '0') * 10 + (value[15] - '0');
    *pollingRate = (value[17] - '0') * 10 + (value[18] - '0');
    *lowThreshold = (value[20] - '0') * 10 + (value[21] - '0');
    *moderateThreshold = (value[23] - '0') * 10 + (value[24] - '0');
    *highThreshold = (value[26] - '0') * 10 + (value[27] - '0');
    *criticalThreshold = (value[29] - '0') * 10 + (value[30] - '0');
}

void parse_SetTime(unsigned char* value, unsigned char *hour, unsigned char *min)
{
    // Format: "HH:mm"
    *hour = (value[0] - '0') * 10 + (value[1] - '0');
    *min = (value[3] - '0') * 10 + (value[4] - '0');
}

unsigned char SIO_SendInt(unsigned char value)
{
    // Envia un enter de 8 bits com a caràcter ASCII
    if (value < 10)
    {
        SIO_SafePrint('0'+value);
    }
    else if (value < 100)
    {
        SIO_SafePrint((value / 10) + '0');
        SIO_SafePrint((value % 10) + '0');
    }
    else
    {
        SIO_SafePrint('9');
        SIO_SafePrint('9');
    }
    return TRUE;
}

// future deprecated functions
unsigned char SIO_TXAvail()
{
    return ((PIR1bits.TXIF == 1) ? TRUE : FALSE);
}
unsigned char SIO_GetChar()
{
    return RCREG;
}
void SIO_PutChar(unsigned char lletra)
{
    TXREG = lletra;
}

void SIO_SafePrint(char lletra)
{ // NO COOPERATIU, NOMES PER DEBUGGING
    while (!SIO_TXAvail())
        ;
    SIO_PutChar(lletra);
}

void SIO_PrintString(const char *text)
{
    while (*text != '\0')
    {
        SIO_SafePrint(*text++);
    }
}

void itoa(unsigned int value, char *str, unsigned char base)
{
    char temp[6]; // Buffer temporal per invertir el n mero (fins a 65535 = 5 xifres + \0)
    int i = 0;

    if (value == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return;
    }

    while (value > 0 && i < 5)
    {
        temp[i++] = (value % base) + '0';
        value /= base;
    }

    // Inverteix el resultat a str
    int j = 0;
    while (i > 0)
    {
        str[j++] = temp[--i];
    }
    str[j] = '\0'; // null terminador
}