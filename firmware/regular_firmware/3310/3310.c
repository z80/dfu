
#include "3310.h"
#include "stm32f10x.h"


/*--------------------------------------------------------------------------------------------------
                                     Character generator

             This table defines the standard ASCII characters in a 5x7 dot format.
--------------------------------------------------------------------------------------------------*/
static const BYTE FontLookup [][5] =
{
    { 0x00, 0x00, 0x00, 0x00, 0x00 },  // sp
    { 0x00, 0x00, 0x2f, 0x00, 0x00 },   // !
    { 0x00, 0x07, 0x00, 0x07, 0x00 },   // "
    { 0x14, 0x7f, 0x14, 0x7f, 0x14 },   // #
    { 0x24, 0x2a, 0x7f, 0x2a, 0x12 },   // $
    { 0xc4, 0xc8, 0x10, 0x26, 0x46 },   // %
    { 0x36, 0x49, 0x55, 0x22, 0x50 },   // &
    { 0x00, 0x05, 0x03, 0x00, 0x00 },   // '
    { 0x00, 0x1c, 0x22, 0x41, 0x00 },   // (
    { 0x00, 0x41, 0x22, 0x1c, 0x00 },   // )
    { 0x14, 0x08, 0x3E, 0x08, 0x14 },   // *
    { 0x08, 0x08, 0x3E, 0x08, 0x08 },   // +
    { 0x00, 0x00, 0x50, 0x30, 0x00 },   // ,
    { 0x10, 0x10, 0x10, 0x10, 0x10 },   // -
    { 0x00, 0x60, 0x60, 0x00, 0x00 },   // .
    { 0x20, 0x10, 0x08, 0x04, 0x02 },   // /
    { 0x3E, 0x51, 0x49, 0x45, 0x3E },   // 0
    { 0x00, 0x42, 0x7F, 0x40, 0x00 },   // 1
    { 0x42, 0x61, 0x51, 0x49, 0x46 },   // 2
    { 0x21, 0x41, 0x45, 0x4B, 0x31 },   // 3
    { 0x18, 0x14, 0x12, 0x7F, 0x10 },   // 4
    { 0x27, 0x45, 0x45, 0x45, 0x39 },   // 5
    { 0x3C, 0x4A, 0x49, 0x49, 0x30 },   // 6
    { 0x01, 0x71, 0x09, 0x05, 0x03 },   // 7
    { 0x36, 0x49, 0x49, 0x49, 0x36 },   // 8
    { 0x06, 0x49, 0x49, 0x29, 0x1E },   // 9
    { 0x00, 0x36, 0x36, 0x00, 0x00 },   // :
    { 0x00, 0x56, 0x36, 0x00, 0x00 },   // ;
    { 0x08, 0x14, 0x22, 0x41, 0x00 },   // <
    { 0x14, 0x14, 0x14, 0x14, 0x14 },   // =
    { 0x00, 0x41, 0x22, 0x14, 0x08 },   // >
    { 0x02, 0x01, 0x51, 0x09, 0x06 },   // ?
    { 0x32, 0x49, 0x59, 0x51, 0x3E },   // @
    { 0x7E, 0x11, 0x11, 0x11, 0x7E },   // A
    { 0x7F, 0x49, 0x49, 0x49, 0x36 },   // B
    { 0x3E, 0x41, 0x41, 0x41, 0x22 },   // C
    { 0x7F, 0x41, 0x41, 0x22, 0x1C },   // D
    { 0x7F, 0x49, 0x49, 0x49, 0x41 },   // E
    { 0x7F, 0x09, 0x09, 0x09, 0x01 },   // F
    { 0x3E, 0x41, 0x49, 0x49, 0x7A },   // G
    { 0x7F, 0x08, 0x08, 0x08, 0x7F },   // H
    { 0x00, 0x41, 0x7F, 0x41, 0x00 },   // I
    { 0x20, 0x40, 0x41, 0x3F, 0x01 },   // J
    { 0x7F, 0x08, 0x14, 0x22, 0x41 },   // K
    { 0x7F, 0x40, 0x40, 0x40, 0x40 },   // L
    { 0x7F, 0x02, 0x0C, 0x02, 0x7F },   // M
    { 0x7F, 0x04, 0x08, 0x10, 0x7F },   // N
    { 0x3E, 0x41, 0x41, 0x41, 0x3E },   // O
    { 0x7F, 0x09, 0x09, 0x09, 0x06 },   // P
    { 0x3E, 0x41, 0x51, 0x21, 0x5E },   // Q
    { 0x7F, 0x09, 0x19, 0x29, 0x46 },   // R
    { 0x46, 0x49, 0x49, 0x49, 0x31 },   // S
    { 0x01, 0x01, 0x7F, 0x01, 0x01 },   // T
    { 0x3F, 0x40, 0x40, 0x40, 0x3F },   // U
    { 0x1F, 0x20, 0x40, 0x20, 0x1F },   // V
    { 0x3F, 0x40, 0x38, 0x40, 0x3F },   // W
    { 0x63, 0x14, 0x08, 0x14, 0x63 },   // X
    { 0x07, 0x08, 0x70, 0x08, 0x07 },   // Y
    { 0x61, 0x51, 0x49, 0x45, 0x43 },   // Z
    { 0x00, 0x7F, 0x41, 0x41, 0x00 },   // [
    { 0x02, 0x04, 0x08, 0x10, 0x20 },   // back slash
    { 0x00, 0x41, 0x41, 0x7f, 0x00 },   // ]
    { 0x04, 0x02, 0x01, 0x02, 0x04 },   // ^
    { 0x40, 0x40, 0x40, 0x40, 0x40 },   // _
    { 0x00, 0x01, 0x02, 0x04, 0x00 },   // '
    { 0x20, 0x54, 0x54, 0x54, 0x78 },   // a
    { 0x7F, 0x48, 0x44, 0x44, 0x38 },   // b
    { 0x38, 0x44, 0x44, 0x44, 0x20 },   // c
    { 0x38, 0x44, 0x44, 0x48, 0x7F },   // d
    { 0x38, 0x54, 0x54, 0x54, 0x18 },   // e
    { 0x08, 0x7E, 0x09, 0x01, 0x02 },   // f
    { 0x0C, 0x52, 0x52, 0x52, 0x3E },   // g
    { 0x7F, 0x08, 0x04, 0x04, 0x78 },   // h
    { 0x00, 0x44, 0x7D, 0x40, 0x00 },   // i
    { 0x20, 0x40, 0x44, 0x3D, 0x00 },   // j
    { 0x7F, 0x10, 0x28, 0x44, 0x00 },   // k
    { 0x00, 0x41, 0x7F, 0x40, 0x00 },   // l
    { 0x7C, 0x04, 0x18, 0x04, 0x78 },   // m
    { 0x7C, 0x08, 0x04, 0x04, 0x78 },   // n
    { 0x38, 0x44, 0x44, 0x44, 0x38 },   // o
    { 0x7C, 0x14, 0x14, 0x14, 0x08 },   // p
    { 0x08, 0x14, 0x14, 0x18, 0x7C },   // q
    { 0x7C, 0x08, 0x04, 0x04, 0x08 },   // r
    { 0x48, 0x54, 0x54, 0x54, 0x20 },   // s
    { 0x04, 0x3F, 0x44, 0x40, 0x20 },   // t
    { 0x3C, 0x40, 0x40, 0x20, 0x7C },   // u
    { 0x1C, 0x20, 0x40, 0x20, 0x1C },   // v
    { 0x3C, 0x40, 0x30, 0x40, 0x3C },   // w
    { 0x44, 0x28, 0x10, 0x28, 0x44 },   // x
    { 0x0C, 0x50, 0x50, 0x50, 0x3C },   // y
    { 0x44, 0x64, 0x54, 0x4C, 0x44 },   // z
    { 0x00, 0x08, 0x36, 0x41, 0x00 },   // {
    { 0x00, 0x00, 0x7f, 0x00, 0x00 },   // |
    { 0x00, 0x41, 0x36, 0x08, 0x00 },   // }
    { 0x04, 0x02, 0x04, 0x08, 0x04 },   // ~
    { 0x00, 0x00, 0x36, 0x00, 0x00 },   // ¦
    { 0x0e, 0x51, 0x31, 0x11, 0x08 },   // Ç
    { 0x3c, 0x41, 0x40, 0x21, 0x7c },   // ü	
    { 0x38, 0x54, 0x56, 0x55, 0x18 },   // é	
    { 0x20, 0x56, 0x55, 0x56, 0x78 },   // â	
    { 0x20, 0x55, 0x54, 0x55, 0x78 },   // ä	
    { 0x20, 0x55, 0x56, 0x54, 0x78 },   // à	
    { 0x08, 0x08, 0x2a, 0x1c, 0x08 },   // Right Arrow  (chr 134)
    { 0x0e, 0x51, 0x31, 0x11, 0x08 },   // ç	
    { 0x38, 0x56, 0x55, 0x56, 0x18 },   // ê	
    { 0x38, 0x55, 0x54, 0x55, 0x18 },   // ë	
    { 0x38, 0x55, 0x56, 0x54, 0x18 },   // è	
    { 0x00, 0x45, 0x7c, 0x41, 0x00 },   // ï	
    { 0x00, 0x46, 0x7d, 0x42, 0x00 },   // î	
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },   // free (chr 141)	
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },   // free (chr 142)		
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },   // free	(chr 143)	
    { 0x7c, 0x54, 0x56, 0x55, 0x44 },   // É	
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },   // free	(chr 145)			
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },   // free	(chr 146)			
    { 0x38, 0x46, 0x45, 0x46, 0x38 },   // ô	
    { 0x7f, 0x7f, 0x7f, 0x7f, 0x7f },   // free	(chr 148)		
    { 0x38, 0x45, 0x46, 0x44, 0x38 },   // ò	
    { 0x3c, 0x42, 0x41, 0x22, 0x7c },   // û
    { 0x3c, 0x41, 0x42, 0x20, 0x7c }    // ù		
};

/*--------------------------------------------------------------------------------------------------
                                      Global Variables
--------------------------------------------------------------------------------------------------*/
static BYTE  g_LcdCache[ LCD_CACHE_SIZE ];

static int   LcdCacheIdx;
BYTE         UpdateLcd;

static void lcdSend( BYTE data );

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdPower ( byte stat )

  Description  :  Performs MCU SPI & LCD controller initialization.

  Argument(s)  :  stat -> true or false

  Return value :  None.
  
  Notes        :  Power ON or OFF LCD

--------------------------------------------------------------------------------------------------*/
void lcdPower( BYTE stat )
{
    if (stat) 
    {
       lcdInit();
    }
    else 
    {
        //PORTB = 0xc0;    // All LCD pin at 0 
        //SPCR = 0x00;     // Disable SPI
        finit3310();
    }

    // For debugging.
    /*while( 1 )
    {
        csLow();
        setModeCmd3310();
        sendByte3310( 0x0F );
        int i=0;
        while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_BSY ) == SET )
            i++;
        i = i + i - i;
        csHigh();

        csLow();
        setModeData3310();
        sendByte3310( 0xF0 );
        int i=0;
        while ( SPI_I2S_GetFlagStatus( SPI1, SPI_I2S_FLAG_BSY ) == SET )
            i++;
        i = i + i - i;
        csHigh();
    }*/
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdInit

  Description  :  Performs MCU SPI & LCD controller initialization.

  Argument(s)  :  None.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdInit( void )
{
    static BYTE FirstInit = BTRUE;
	
    init3310( g_LcdCache, LCD_CACHE_SIZE );
    //  Toggle display reset pin.
    delay3310( 65536 * 4 );
    /* Deselect the display Chip Select high */
    rstLow();
    delay3310( 65536 * 4 );
    rstHigh();
    delay3310( 65536 * 4 );
 
    //  Disable LCD controller
    //PORTB |= LCD_CE_PIN;
    //csLow();
    //setModeCmd3310();
    //lcdSend( 0x21 );  // LCD Extended Commands.
    //lcdSend( 0xC8 );  // Set LCD Vop (Contrast).
    //lcdSend( 0x06 );  // Set Temp coefficent.
    //lcdSend( 0x13 );  // LCD bias mode 1:48.
    //lcdSend( 0x20 );  // LCD Standard Commands, Horizontal addressing mode.
    //lcdSend( 0x0C );  // LCD in normal mode.
    /*
        LcdBiasSystem(0x03);    //ÍÆ¼ö»ìºÏÂÊ 1:48
        LcdSetVop(0x32);                //V(6.06) = 3.06 + 0.06*Vop
        LcdTempCtrl(0x00);              //ÎÂ¶ÈÏµÊý 0~3
        LcdFunctionSet(ACTIVE, H_ADDR, BAS_INS);        //»ù±¾Ö¸Áî¼¯
        LcdDispalyControl((u8)NORMAL_MODE);             //ÆÕÍ¨Ä£Ê½ÏÔÊ¾
        //
        LcdWriteDC(LCDCMD , 0x21 );
        LcdWriteDC(LCDCMD , 0xc8 );
        LcdWriteDC(LCDCMD , 0x20 );
        LcdWriteDC(LCDCMD , 0x0c );
     */

    lcdFuncSet( 0, THorizontal, TExtended );
    lcdVop( 0x48 );
    lcdBias( 3 );
    lcdTempCtrl( TTemp0 );

    lcdFuncSet( 0, THorizontal, TBasic );
    lcdDispCtrl( TNormalMode );

    if (FirstInit == BTRUE)
    {
        lcdClear();
        FirstInit = BFALSE;
    }
    lcdUpdate();
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdContrast

  Description  :  Set display contrast.

  Argument(s)  :  contrast -> Contrast value from 0x00 to 0x7F.

  Return value :  None.

  Notes        :  No change visible at ambient temperature.

--------------------------------------------------------------------------------------------------*/
void lcdContrast( BYTE contrast )
{
    setModeCmd3310();
    
    //  LCD Extended Commands.
    lcdSend( 0x21 );

    // Set LCD Vop (Contrast).
    lcdSend( 0x80 | contrast );

    //  LCD Standard Commands, horizontal addressing mode.
    lcdSend( 0x20 );
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdClear

  Description  :  Clears the display. LcdUpdate must be called next.

  Argument(s)  :  None.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdClear( void )
{
    int i;
    
    for ( i=0; i<LCD_CACHE_SIZE; i++ )
        g_LcdCache[i] = 0x00;
    
    UpdateLcd = BTRUE;
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdGotoXY

  Description  :  Sets cursor location to xy location corresponding to basic font size.

  Argument(s)  :  x, y -> Coordinate for new cursor position. Range: 1,1 .. 14,6

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdGotoXy( BYTE x, BYTE y )
{
    LcdCacheIdx = (x - 1) * 6 + (y - 1) * 84;
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdChr

  Description  :  Displays a character at current cursor location and increment cursor location.

  Argument(s)  :  size -> Font size. See enum.
                  ch   -> Character to write.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdChr( LcdFontSize size, BYTE ch )
{
    BYTE i, c;
    BYTE b1, b2;
    int  tmpIdx;

    if (ch < 0x20) 
    {
        ch = 148;
    }
	
    if (ch > 151) // Convert ISO8859-1 to ascii
    {
        if (ch == 0xc0) ch = 133;
        else if (ch == 0xc2) ch = 131;
        else if (ch == 0xc7) ch = 128;
        else if (ch == 0xc9) ch = 144;
        else if (ch == 0xca) ch = 136;
        else if (ch == 0xce) ch = 140;
        else if (ch == 0xe0) ch = 133;
        else if (ch == 0xe2) ch = 131;
        else if (ch == 0xe7) ch = 135;
        else if (ch == 0xe8) ch = 138;
        else if (ch == 0xe9) ch = 130;
        else if (ch == 0xea) ch = 136;
        else if (ch == 0xeb) ch = 137;
        else if (ch == 0xee) ch = 140;
        else if (ch == 0xef) ch = 139;
        else if (ch == 0xf4) ch = 147;
        else if (ch == 0xf9) ch = 151;
        else if (ch == 0xfb) ch = 150;
        else ch = 148;
    }

    if ( size == FONT_1X )
    {
        for ( i=0; i<5; i++ )
        {
            g_LcdCache[LcdCacheIdx++] = FontLookup[ch - 32][i] << 1;
        }
    }
    else if ( size == FONT_2X )
    {
        tmpIdx = LcdCacheIdx - 84;

        if ( tmpIdx < 0 )
            return;

        for ( i = 0; i < 5; i++ )
        {
            c = FontLookup[ch - 32][i] << 1;
            b1 =  (c & 0x01) * 3;
            b1 |= (c & 0x02) * 6;
            b1 |= (c & 0x04) * 12;
            b1 |= (c & 0x08) * 24;

            c >>= 4;
            b2 =  (c & 0x01) * 3;
            b2 |= (c & 0x02) * 6;
            b2 |= (c & 0x04) * 12;
            b2 |= (c & 0x08) * 24;

            g_LcdCache[tmpIdx++] = b1;
            g_LcdCache[tmpIdx++] = b1;
            g_LcdCache[tmpIdx + 82] = b2;
            g_LcdCache[tmpIdx + 83] = b2;
        }
        
        //  Update x cursor position.
        LcdCacheIdx += 11;
    }
    
    //  Horizontal gap between characters.
    g_LcdCache[LcdCacheIdx++] = 0x00;
    UpdateLcd = BTRUE;
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdStr

  Description  :  Displays a character at current cursor location and increment cursor location
                  according to font size.

  Argument(s)  :  size    -> Font size. See enum.
                  dataPtr -> Pointer to null terminated ASCII string to display.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdStr( LcdFontSize size, BYTE * dataPtr )
{
    while( *dataPtr )
        lcdChr( size, *dataPtr++ );
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdStrConst

  Description  :  Displays a character at current cursor location and increment cursor location
                  according to font size.

  Argument(s)  :  size    -> Font size. See enum.
                  dataPtr -> Pointer to null terminated ASCII string to display.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdStrConst( LcdFontSize size, const char * dataPtr )
{
    while ( *dataPtr )
        lcdChr( size, *dataPtr++ );
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdPixel

  Description  :  Displays a pixel at given absolute (x, y) location.

  Argument(s)  :  x, y -> Absolute pixel coordinates
                  mode -> Off, On or Xor. See enum.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdPixel( BYTE x, BYTE y, LcdPixelMode mode )
{
    WORD  index;
    BYTE  offset;
    BYTE  data;
    
    if ( x > LCD_X_RES ) return;
    if ( y > LCD_Y_RES ) return;
    
    index = ((y / 8) * 84) + x;
    offset  = y - ((y / 8) * 8);
    
    data = g_LcdCache[index];
    
    if ( mode == PIXEL_OFF )
    {
        data &= (~(0x01 << offset));
    }
    else if ( mode == PIXEL_ON )
    {
        data |= (0x01 << offset);
    }
    else if ( mode  == PIXEL_XOR )
    {
        data ^= (0x01 << offset);
    }
    
    g_LcdCache[index] = data;
    UpdateLcd = BTRUE;
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdLine

  Description  :  Draws a line between two points on the display.

  Argument(s)  :  x1, y1 -> Absolute pixel coordinates for line origin.
                  x2, y2 -> Absolute pixel coordinates for line end.
                  mode   -> Off, On or Xor. See enum.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdLine( BYTE x1, BYTE y1, BYTE x2, BYTE y2, LcdPixelMode mode )
{
    int dx, dy, stepx, stepy, fraction;

    dy = y2 - y1;
    dx = x2 - x1;

    if ( dy < 0 )
    {
        dy    = -dy;
        stepy = -1;
    }
    else
    {
        stepy = 1;
    }

    if ( dx < 0 )
    {
        dx    = -dx;
        stepx = -1;
    }
    else
    {
        stepx = 1;
    }

    dx <<= 1;
    dy <<= 1;

    lcdPixel( x1, y1, mode );

    if ( dx > dy )
    {
        fraction = dy - (dx >> 1);
        while ( x1 != x2 )
        {
            if ( fraction >= 0 )
            {
                y1 += stepy;
                fraction -= dx;
            }
            x1 += stepx;
            fraction += dy;
            lcdPixel( x1, y1, mode );
        }
    }
    else
    {
        fraction = dx - (dy >> 1);
        while ( y1 != y2 )
        {
            if ( fraction >= 0 )
            {
                x1 += stepx;
                fraction -= dy;
            }
            y1 += stepy;
            fraction += dx;
            lcdPixel( x1, y1, mode );
        }
    }

    UpdateLcd = BTRUE;
}

/*--------------------------------------------------------------------------------------------------

  Name         :  LcdUpdate

  Description  :  Copies the LCD cache into the device RAM.

  Argument(s)  :  None.

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
void lcdUpdate( void )
{
    csHigh();

   //  Set base address according to LoWaterMark.
    lcdFuncSet( 0, THorizontal, TBasic );
    lcdYAddr( 0 );
    lcdXAddr( 0 );

    //  Serialize the video buffer.
    setModeData3310();
    int i;
    for ( i=0; i<LCD_CACHE_SIZE; i++ )
        sendByte3310( g_LcdCache[i] );

    //waitArrayReady3310();    
    //sendArray3310( g_LcdCache, LCD_CACHE_SIZE );
    // For debug wait ready right here.
    //waitArrayReady3310();

    UpdateLcd = BFALSE;
}



/*--------------------------------------------------------------------------------------------------

  Name         :  LcdSend

  Description  :  Sends data to display controller.

  Argument(s)  :  data -> Data to be sent
                  cd   -> Command or data (see/use enum)

  Return value :  None.

--------------------------------------------------------------------------------------------------*/
static void lcdSend( BYTE data )
{
    sendByte3310( data );
/*
    //  Enable display controller (active low).
    PORTB &= ~LCD_CE_PIN;

    if ( cd == LCD_DATA )
    {
        PORTB |= LCD_DC_PIN;
    }
    else
    {
        PORTB &= ~LCD_DC_PIN;
    }

    //  Send data to display controller.
    SPDR = data;

    //  Wait until Tx register empty.
    while ( (SPSR & 0x80) != 0x80 );

    //  Disable display controller.
    PORTB |= LCD_CE_PIN;
	
	// Only for 38Khz share pin for remote controle
	PORTB &= ~LCD_DC_PIN;
*/
}

static void lcdCmd( BYTE cmd )
{
	csLow();
	setModeCmd3310();
	sendByte3310( cmd );
}

void lcdFuncSet( BYTE powerDown,
		         LcdAddressing addr,
		         TInstructionSet instSet )
{
	BYTE val = (1 << 5);
	if ( powerDown )
		val |= (1 << 2);
	if ( addr == TVertical )
		val |= (1<<1);
	if ( instSet == TExtended )
		val |= 1;
	lcdCmd( val );
}

void lcdDispCtrl( TDisplayCtrl ctrl )
{
	BYTE val = (1 << 3);
	if ( ctrl == TNormalMode )
		val |= ( 1 << 2 );
	else if ( ctrl == TAllOn )
		val |= 1;
	else if ( ctrl == TInverseMode )
		val |= (( 1 << 2 ) | 1 );
	lcdCmd( val );
}

void lcdYAddr( BYTE y )
{
    BYTE val = (1 << 6);
    val |= y & 0x07;
    lcdCmd( val );
}

void lcdXAddr( BYTE x )
{
    BYTE val = (1 << 7);
    val |= x & 0x7F;
    lcdCmd( val );
}

void lcdTempCtrl( TTempCoef t )
{
	BYTE val = (1 << 2);
	val |= (t & 0x03);
	lcdCmd( val );
}

void lcdBias( BYTE bias )
{
	BYTE val = (1 << 4);
	val |= ( bias & 0x07);
	lcdCmd( val );
}

void lcdVop( BYTE vop )
{
	BYTE val = (1 << 7);
	val |= (vop & 0x7F);
}






