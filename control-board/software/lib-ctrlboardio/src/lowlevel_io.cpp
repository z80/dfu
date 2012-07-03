
#include "lowlevel_io.h"
#include "ctrlboard_functions.h"
#include "string.h"

inline static bool LSB()
{
    int i = 1;
    unsigned char * p = reinterpret_cast<unsigned char *>( &i );
    bool res = (p[0] != 0);
    return res;
}

LowlevelIo::LowlevelIo()
: UsbIo()
{
}

LowlevelIo::~LowlevelIo()
{
}

int LowlevelIo::putArgs( int size, unsigned char * args )
{
    std::basic_string<unsigned char> & data = this->data();
    int sz = size + 2;
    data.resize( sz );
    data[0] = CMD_DATA;
    data[1] = static_cast<unsigned char>( size );
    unsigned char * d = args;
    int k = 2;
    for ( int i=0; i<size; i++ )
        data[k++] = d[i];
    int res = write( const_cast<unsigned char *>( data.data() ), sz );
    return res;
}

int LowlevelIo::putString( char * stri )
{
    int sz = strlen( stri );
    int res = putArgs( sz, reinterpret_cast<unsigned char *>( stri ) );
    return res;
}

int LowlevelIo::putUInt8( unsigned char val )
{
    int res = putArgs( sizeof(unsigned char), &val );
    return res;
}

// Here it is necessary to revert numbers to make things compatible with target's bytes order.
int LowlevelIo::putUInt16( unsigned short val )
{
    unsigned char * p = reinterpret_cast<unsigned char *>( &val );
    if ( LSB() )
    {
        int res = putArgs( sizeof(unsigned short), p );
        return res;
    }
    else
    {
        unsigned char b[ sizeof(unsigned short) ];
        b[0] = p[1];
        b[1] = p[0];
        int res = putArgs( sizeof(unsigned short), b );
        return res;
    }
}

int LowlevelIo::putUInt32( unsigned long val )
{
    unsigned char * p = reinterpret_cast<unsigned char *>( &val );
    if ( LSB() )
    {
        int res = putArgs( sizeof(unsigned long), p );
        return res;
    }
    else
    {
        unsigned char b[ sizeof(unsigned long) ];
        b[0] = p[3];
        b[1] = p[2];
        b[2] = p[1];
        b[3] = p[0];
        int res = putArgs( sizeof(unsigned long), b );
        return res;
    }
}

int LowlevelIo::execFunc( int index )
{
    std::basic_string<unsigned char> & data = this->data();
    data.clear();
    int sz = 2;
    data.resize( sz );
    data[0] = CMD_FUNC;
    data[1] = static_cast<unsigned char>( index );
    int res = write( const_cast<unsigned char *>( data.data() ), data.size() );
    return res;
}







