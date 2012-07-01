
#include "ctrlboard_io.h"

inline static bool LSB()
{
    int i = 1;
    unsigned char * p = reinterpret_cast<unsigned char *>( &i );
    bool res = (p[0] != 0);
    return res;
}

inline static int toUInt16( unsigned char d[] )
{
    int res;
    if ( LSB() )
        res = (static_cast<int>( d[0] ) << 8) + d[1];
    else
        res = (static_cast<int>( d[1] ) << 8) + d[0];
    return res;
}

CtrlboardIo::CtrlboardIo()
: LowlevelIo()
{
}

CtrlboardIo::~CtrlboardIo()
{
}

int  CtrlboardIo::version()
{
    execFunc( FUNC_VERSION );
    unsigned char d[2];
    int sz = readQueue( d, 2 );
    if ( sz < 2 )
        return -1;
    int res = d[0] + d[1] * 256;
    return res;
}

bool CtrlboardIo::gpioEn( int index, bool en )
{
	putUInt8( static_cast<unsigned char>( index ) );
	putUInt8( ( en ) ? 1 : 0 );
	execFunc( FUNC_GPIO_EN );
	return true;
}

bool CtrlboardIo::gpioConfig( int index, int pins, int mode )
{
    putUInt8( static_cast<unsigned char>( index ) );
    putUInt16( static_cast<unsigned short>( pins ) );
    putUInt8( static_cast<unsigned char>( mode ) );
    execFunc( FUNC_GPIO_CONFIG );
    return true;
}

bool CtrlboardIo::gpioSet( int index, int pins, int vals )
{
    putUInt8( static_cast<unsigned char>( index ) );
    putUInt16( static_cast<unsigned short>( pins ) );
    putUInt16( static_cast<unsigned short>( vals ) );
    execFunc( FUNC_GPIO_SET );
    return true;
}

bool CtrlboardIo::gpio( int index, int pins, int & vals )
{
    putUInt8( static_cast<unsigned char>( index ) );
    putUInt8( static_cast<unsigned char>( pins ) );
    execFunc( FUNC_GPIO );
    unsigned char d[2];
    int sz = readQueue( d, 2 );
    if ( sz < 2 )
        return false;
    vals = d[0] + d[1] * 256;
    return true;
}

bool CtrlboardIo::adcEnable( int pins )
{
    /*putUInt8( 0, static_cast<unsigned char>( index ) );
    putUInt8( 1, val ? 1 : 0 );
    execFunc( CMD_ADC_ENABLE );*/
    return true;
}

bool CtrlboardIo::adc( std::basic_string<int> & vals )
{
    /*putUInt8( 0, static_cast<unsigned char>( index ) );
    execFunc( CMD_ADC );
    int sz = queueSize();
    unsigned char d[2];
    int cnt = readQueue( d, 2 );
    if ( cnt < 2 )
        return -1;
    int res = toUInt16( d );
    return res;*/
    return 1;
}


bool CtrlboardIo::twiEnable( bool val )
{
    return true;
}

bool CtrlboardIo::twiSetAddress( int addr )
{
    return true;
}

bool CtrlboardIo::twiSetSpeed( int val )
{
    return true;
}

bool CtrlboardIo::twiWriteRead( int addr, TIo wr, TIo & rd )
{
    return true;
}






