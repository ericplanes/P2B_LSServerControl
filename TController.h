#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include "Utils.h"

// System status states
#define CTR_LOW 0
#define CTR_MOD 1
#define CTR_HIGH 2
#define CTR_CRIT 3

BYTE CTR_GetStatus(void);

#endif
