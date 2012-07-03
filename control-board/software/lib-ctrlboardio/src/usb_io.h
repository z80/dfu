
#ifndef __USB_IO_H_
#define __USB_IO_H_

#include <string>

class UsbIo
{
public:
    UsbIo();
    virtual ~UsbIo();
    bool open( const std::string & arg = std::string() );
    void close();
    bool isOpen() const;

    // Low level read/write functionality.
    int write( unsigned char * data, int size );
    int read( unsigned char * data, int maxSize );
    int setTimeout( int ms );

    int readQueue( unsigned char * data, int maxSize );
    
    std::basic_string<unsigned char> & data();
protected:
    class PD;
    PD * pd;
};

#endif



