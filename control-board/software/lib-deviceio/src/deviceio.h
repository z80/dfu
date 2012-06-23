
#ifndef __DEVICEIO_H_
#define __DEVICEIO_H_

#include "usb_io.h"
#include "device_functions.h"

class DeviceIo: public UsbIo
{
public:
    typedef std::basic_string< unsigned char > TIo;

    DeviceIo();
    ~DeviceIo();

    bool adcEnable( int index, bool val );
    int adc( int index );

    bool gpioEnable( int index, bool val );
    bool gpioSetMode( int index, int mode );
    bool gpioSet( int index, bool val );
    bool gpio( int index, bool & val );

    bool twiEnable( bool val );
    bool twiSetAddress( int addr );
    bool twiSetSpeed( int val );
    bool twiWriteRead( int addr, TIo wr, TIo & rd );
};



#endif


