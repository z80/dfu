
#ifndef __USB_IO_H_
#define __USB_IO_H_

#include <string>

class UsbIo
{
public:
    UsbIo();
    virtual ~UsbIo();
    bool open();
    void close();
    bool isOpen() const;

    // Low level read/write functionality.
    int write( unsigned char * data, int size );
    int read( unsigned char * data, int maxSize );
    int setTimeout( int ms );

    int putArgs( int size, unsigned char * data );
    int putString( char * stri );
    int putUInt8( unsigned char val );
    int putUInt16( unsigned short val );
    int putUInt32( unsigned long val );

    int execFunc( int index );
    //int queueSize();
    int readQueue( unsigned char * data, int maxSize );

protected:
    class PD;
    PD * pd;
};

#endif



