
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
    int write( void * data, int size );
    int read( void * data, int maxSize );

    int putArgs( int start, int size, void * data );
    int putString( int start, char * stri );
    int putUInt8( int start, unsigned char val );
    int putUInt16( int start, unsigned short val );
    int putUInt32( int start, unsigned long val );

    int execFunc( int index );
    int queueSize();
    int readQueue( void * data, int maxSize );

    int setTimeout( int ms );
protected:
    static const int VENDOR_ID;
    static const int PRODUCT_ID;
    static const int TIMEOUT;
    class PD;
    PD * pd;
};

#endif



