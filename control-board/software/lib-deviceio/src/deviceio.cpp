
#include "deviceio.h"

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

DeviceIo::DeviceIo()
: UsbIo()
{
}

DeviceIo::~DeviceIo()
{
}

bool DeviceIo::adcEnable( int index, bool val )
{
    putUInt8( 0, static_cast<unsigned char>( index ) );
    putUInt8( 1, val ? 1 : 0 );
    execFunc( CMD_ADC_ENABLE );
    return true;
}

int DeviceIo::adc( int index )
{
    putUInt8( 0, static_cast<unsigned char>( index ) );
    execFunc( CMD_ADC );
    int sz = queueSize();
    unsigned char d[2];
    int cnt = readQueue( reinterpret_cast<void *>( d ), 2 );
    if ( cnt < 2 )
        return -1;
    int res = toUInt16( d );
    return res;
}

bool DeviceIo::gpioEnable( int index, bool val )
{
    putUInt8( 0, static_cast<unsigned char>( index ) );
    putUInt8( 1, val ? 1 : 0 );
    execFunc( CMD_GPIO_ENABLE );
    return true;
}

bool DeviceIo::gpioSetMode( int index, int mode )
{
    putUInt8( 0, static_cast<unsigned char>( index ) );
    putUInt8( 1, static_cast<unsigned char>( mode ) );
    execFunc( CMD_GPIO_ENABLE );
    return true;
}

bool DeviceIo::gpioSet( int index, bool val )
{
    putUInt8( 0, static_cast<unsigned char>( index ) );
    putUInt8( 1, static_cast<unsigned char>( val ? 1 : 0 ) );
    execFunc( CMD_GPIO_SET );
    return true;
}

bool DeviceIo::gpio( int index, bool & val )
{
    putUInt8( 0, static_cast<unsigned char>( index ) );
    execFunc( CMD_GPIO );
    int cnt = queueSize();
    unsigned char d;
    int sz = readQueue( reinterpret_cast<void *>( &d ), 1 );
    if ( cnt < 1 )
        return false;
    val = (d != 0);
    return true;
}

bool DeviceIo::twiEnable( bool val )
{
    return true;
}

bool DeviceIo::twiSetAddress( int addr )
{
    return true;
}

bool DeviceIo::twiSetSpeed( int val )
{
    return true;
}

bool DeviceIo::twiWriteRead( int addr, TIo wr, TIo & rd )
{
    return true;
}



