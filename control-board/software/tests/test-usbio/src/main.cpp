
#include "usb_io.h"
#include "deviceio.h"

int main( int argc, char * argv[] )
{
    DeviceIo io;
    bool res = io.open();
    const int CNT = 64;
    unsigned char b[ CNT ];
    //int cnt = io.write( b, CNT );
    //cnt = io.read( b, CNT );
    int v = io.version();
    io.close();
    return 0;
}




