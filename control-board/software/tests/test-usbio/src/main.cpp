
#include "usb_io.h"
#include "deviceio.h"

int main( int argc, char * argv[] )
{
    DeviceIo io;
    bool res = io.open();
    const int CNT = 64;
    unsigned char b[ CNT ];
    int cnt;
    do {
        cnt = io.write( b, 2 );
        //cnt = io.read( b, 2 );
    } while ( cnt > 0 );
    //int v = io.version();
    io.close();
    return 0;
}




