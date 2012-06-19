
#ifndef __3310_H_
#define __3310_H_

#include "3310-backend.h"

#define NULL                       0
#define BFALSE                      0
#define BTRUE                       1

#define LCD_X_RES                  84
#define LCD_Y_RES                  48
#define LCD_CACHE_SIZE             ((LCD_X_RES * LCD_Y_RES) / 8)

typedef enum
{
    LCD_CMD  = 0,
    LCD_DATA = 1

} LcdCmdData;

typedef enum
{
    PIXEL_OFF =  0,
    PIXEL_ON  =  1,
    PIXEL_XOR =  2

} LcdPixelMode;

typedef enum
{
    FONT_1X = 1,
    FONT_2X = 2

} LcdFontSize;

void lcdInit       ( void );
void lcdPower 	   ( BYTE en );
void lcdContrast   ( BYTE contrast );
void lcdClear      ( void );
void lcdUpdate     ( void );
void lcdGotoXy     ( BYTE x, BYTE y );
void lcdChr        ( LcdFontSize size, BYTE ch );
void lcdStr        ( LcdFontSize size, BYTE * dataPtr );
void lcdStrConst   ( LcdFontSize size, const char * dataPtr );
void lcdPixel      ( BYTE x, BYTE y, LcdPixelMode mode );
void lcdLine       ( BYTE x1, BYTE y1, BYTE x2, BYTE y2, LcdPixelMode mode );

typedef enum
{
    THorizontal = 0,
    TVertical
} LcdAddressing;

typedef enum
{
	TBasic = 0,
	TExtended
} TInstructionSet;

typedef enum
{
	TBlank = 0,
	TNormalMode,
	TAllOn,
	TInverseMode
} TDisplayCtrl;

typedef enum
{
   TTemp0 = 0,
   TTemp1,
   TTemp2,
   TTemp3
}  TTempCoef;

// Display tuning routines.
void lcdFuncSet( BYTE powerDown,
		         LcdAddressing addr,
		         TInstructionSet instSet );
void lcdDispCtrl( TDisplayCtrl ctrl );
void lcdYAddr( BYTE y );
void lcdXAddr( BYTE x );
void lcdTempCtrl( TTempCoef t );
void lcdBias( BYTE bias );
void lcdVop( BYTE vop ); // Contrast

#endif



