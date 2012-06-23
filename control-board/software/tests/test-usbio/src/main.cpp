
#include "usb_io.h"
#include "deviceio.h"

int main( int argc, char * argv[] )
{
    UsbIo io;
    bool res = io.open();
    const int CNT = 64;
    unsigned char b[ CNT ];
    int cnt = io.write( reinterpret_cast<void *>(b), CNT );
    cnt = io.read( reinterpret_cast<void *>(b), CNT );
    io.close();
    return 0;
}




