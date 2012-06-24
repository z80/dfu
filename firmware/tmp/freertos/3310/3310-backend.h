
#ifndef __3310_BACKEND_H_
#define __3310_BACKEND_H_

typedef char                       BOOL;
typedef unsigned char              BYTE;
typedef unsigned int               WORD;

void delay3310( int cnt );

void init3310( BYTE * data, int cnt );
void finit3310(void);
void sendByte3310( BYTE byte );
void sendArray3310( BYTE * data, int cnt );
void waitArrayReady3310( void );
void setModeCmd3310( void );
void setModeData3310( void );

void dcLow(void);
void dcHigh(void);
void csLow(void);
void csHigh(void);
void rstLow(void);
void rstHigh(void);
void pwrLow(void);
void pwrHigh(void);

#endif


