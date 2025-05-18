#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include "TTimer.h"
#include "TTemperature.h"
#include "TMenu.h"
#include "TI2C.h"
#include "TRAM.h"
#include "TEEPROM.h"

#include "TAD_SIO.h"
#include "Utils.h"

void CTR_Init(void);
void CTR_Motor(void);
SYS_STATUS CTR_GetStatus(void);
BYTE *CTR_TEST_GetInfo(void);

// Testing
BYTE *CTR_TEST_GetInfo(void);

#endif
