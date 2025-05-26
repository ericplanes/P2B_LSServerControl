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