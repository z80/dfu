
#ifndef __DEVICEIO_H_
#define __DEVICEIO_H_

#include "usb_io.h"
#include "device_functions.h"
#include <string>

class DeviceIo: public UsbIo
{
public:
    typedef std::basic_string< unsigned char > TIo;

    DeviceIo();
    ~DeviceIo();

    int version();

    bool gpioEn( int index, bool en );
    bool gpioConfig( int index, int pins, int mode );
    bool gpioSet( int index, int pins, int vals );
    bool gpio( int index, int pins, int & val );

    bool adcEnable( int pins );
    bool adc( std::basic_string<int> & vals );

    bool twiEnable( bool val );
    bool twiSetAddress( int addr );
    bool twiSetSpeed( int val );
    bool twiWriteRead( int addr, TIo wr, TIo & rd );
};



#endif


