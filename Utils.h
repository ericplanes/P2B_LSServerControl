#ifndef _UTILS_H_
#define _UTILS_H_

#define FALSE 0
#define TRUE 1

#define BOOL unsigned char   // returns 0 or 1
#define STATUS unsigned char // returns a define with STATUS at the begining
#define BYTE unsigned char
#define WORD unsigned short

#define TIMESTAMP_SIZE sizeof("hhmmssDDMMYYYY")

// Diferent status that we have
#define STATUS_OFF 0
#define STATUS_LOW 1
#define STATUS_MOD 2
#define STATUS_HIGH 3
#define STATUS_CRIT 4

#endif
