
#include "mcu_ctrl.h"

#include <iostream>
#include <sstream>
#include <boost/regex.hpp>

#ifndef WIN32
    #include <unistd.h>
    
#else
    #include <windows.h>
#endif

unsigned char OWI_ComputeCRC8(unsigned char inData, unsigned char seed)
{
    unsigned char bitsLeft;
    unsigned char temp;
    for (bitsLeft = 8; bitsLeft > 0; bitsLeft--)
    {
        temp = ((seed ^ inData) & 0x01);
        if (temp == 0)
        {
            seed >>= 1;
        }
        else
        {
            seed ^= 0x18;
            seed >>= 1;
            seed |= 0x80;
        }
        inData >>= 1;
    }
    return seed;
}

int main( int argc, char * argv[] )
{

    unsigned char crc;
    crc = OWI_ComputeCRC8( 1, 0 );
    crc = OWI_ComputeCRC8( 2, crc );
    return 0;
    /*std::string stri = "ok:{0 34 54}<\r\n";
    std::string::const_iterator start = stri.begin();
    std::string::const_iterator end   = stri.end();
    boost::regex patt( "\\{([\\d\\s]{1,})\\}" );
    boost::smatch match;
    while ( boost::regex_search( start, end, match, patt ) )
    {
        // Entire match.
        std::string all = match[0];
        std::cout << match[0] << "\n";
        // First result.
        std::string first = match[1];
        std::cout << match[1] << "\n";
        // Second result ...
        // Third  result ...
    }*/


    McuCtrl c;
    bool res = c.open();
    if ( !res )
        return 1;
    /*
    // Accelerometer.
    unsigned char accAddr = 25,
                  magAddr = 30;
    unsigned char data[60];

    // Init acc.
    res = c.i2cSetAddr( accAddr );
    data[0] = 0x20;
    data[1] = 0b00100111; //0x27;
    res = c.i2cSetBuf( 0, data, 2 );
    res = c.i2cIo( 2, 0 );
    usleep( 100 * 1000 );
    int status = -1;
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );

    res = c.i2cSetAddr( accAddr );
    data[0] = 0x23;
    data[1] = 0b00000000;
    res = c.i2cSetBuf( 0, data, 2 );
    res = c.i2cIo( 2, 0 );
    usleep( 100 * 1000 );
    status = -1;
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );

    // Init mag.
    res = c.i2cSetAddr( magAddr );
    data[0] = 0x00;
    data[1] = 0b10010000; //0x00;
    res = c.i2cSetBuf( 0, data, 2 );
    res = c.i2cIo( 2, 0 );
    usleep( 100 * 1000 );
    status = -1;
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );

    data[0] = 0x02;
    data[1] = 0b00000000; //0x00;
    res = c.i2cSetBuf( 0, data, 2 );
    res = c.i2cIo( 2, 0 );
    usleep( 100 * 1000 );
    status = -1;
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );
    res = c.i2cStatus( status );

    int g[7];

    // Acc read.
    while ( true )
    {
        res = c.i2cSetAddr( accAddr );
        // Reading back.
        data[0] = (0x28 | (1<<7));
        res = c.i2cSetBuf( 0, data, 1 );
        res = c.i2cIo( 1, 6 );
        usleep( 100 * 1000 );
        res = c.i2cStatus( status );
        res = c.i2cStatus( status );
        res = c.i2cStatus( status );
        res = c.i2cBuffer( 6, data );
        for ( int j=0; j<3; j++ )
        {
            g[j] = ( static_cast<int>(data[j*2]) | (static_cast<int>(data[j*2+1]) * 256) );
            if ( g[j] & (1<<15) )
                g[j] -= 65536;
        }

        res = c.i2cSetAddr( magAddr );
        // Mag read.
        data[0] = 0x03;
        res = c.i2cSetBuf( 0, data, 1 );
        res = c.i2cIo( 1, 6 );
        usleep( 100 * 1000 );
        res = c.i2cStatus( status );
        res = c.i2cStatus( status );
        res = c.i2cStatus( status );
        res = c.i2cBuffer( 6, data );
        for ( int k=3; k<6; k++ )
        {
            int j = k - 3;
            g[k] = (static_cast<int>(data[j*2]) * 256) | static_cast<int>(data[j*2+1]);
            if ( g[k] & (1<<15) )
                g[k] -= 65536;
        }
        // The order is x, z, y.
        int buf;
        buf = g[4];
        g[4] = g[5];
        g[5] = buf;

        // Temp read.
        data[0] = 0x31;
        res = c.i2cSetBuf( 0, data, 1 );
        res = c.i2cIo( 1, 2 );
        usleep( 100 * 1000 );
        res = c.i2cStatus( status );
        res = c.i2cStatus( status );
        res = c.i2cStatus( status );
        res = c.i2cBuffer( 2, data );
        g[6] = static_cast<int>( data[0]) * 256 + static_cast<int>(data[1]);

        for ( int j=0; j<7; j++ )
        {
            std::cout << g[j];
            if ( j<6 )
                std::cout << ",\t";
            else
                std::cout <<"\n";
        }

        // Delay.
        usleep( 500 * 1000 );
    }
    // / Accelerometer.
    */


    unsigned char addr = 10;

    res = c.i2cSetAddr( addr );
    res = c.i2cSetAddr( addr );
    unsigned char data[6];
    data[0] = 6;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    res = c.i2cSetBuf( 0, data, 4 );
    int status = -1;
    res = c.i2cStatus( status );

    status = -1;
        res = c.i2cIo( 4, 0 );
        res = c.i2cStatus( status );

        res = c.i2cIo( 0, 4 );
        res = c.i2cStatus( status );
        res = c.i2cBuffer( 4, data );

        res = c.i2cIo( 4, 4 );
        res = c.i2cStatus( status );
        res = c.i2cBuffer( 4, data );


        res = c.i2cBuffer( 4, data );
        std::cout << data[0] << " " << data[1] << " " << data[2] << " " << data [3] << "\n";

    return 0;
}

