
#include "usb_io.h"
#include "deviceio.h"

int main( int argc, char * argv[] )
{
    DeviceIo io;
    bool res = io.open();
    const int CNT = 64;
    unsigned char b[ CNT ];
    int cnt;
    for ( int i=1; i<5; i++ )
    {
    	b[0] = i;
        cnt = io.write( b, 1 );
        cnt = io.read( &b[1], 3 );
    }
    //int v = io.version();
    io.close();
    return 0;
}




