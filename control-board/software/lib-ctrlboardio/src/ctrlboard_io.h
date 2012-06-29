
#ifndef __CTRLBOARD_IO_H_
#define __CTRLBOARD_IO_H_

#include "lowlevel_io.h"
#include "ctrlboard_functions.h"
#include <string>

class CtrlboardIo: public LowlevelIo
{
public:
    typedef std::basic_string< unsigned char > TIo;

    CtrlboardIo();
    ~CtrlboardIo();

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


