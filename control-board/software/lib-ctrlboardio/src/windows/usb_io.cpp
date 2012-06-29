

#include "usb_io.h"
#include <string.h>
extern "C"
{
    #include "lusb0_usb.h"
    #include "opendevice.h"
}

inline static bool LSB()
{
    int i = 1;
    unsigned char * p = reinterpret_cast<unsigned char *>( &i );
    bool res = (p[0] != 0);
    return res;
}

class UsbIo::PD
{
public:
    PD() {}
    ~PD() {}
    usb_dev_handle * handle;
    std::string err;
    std::string res;
    std::string output;
    int timeout;
    std::basic_string<unsigned char> data;
    static const int VENDOR_ID;
    static const int PRODUCT_ID;
    static const int TIMEOUT;
    static const int EP_OUT;
    static const int EP_IN;
};

const int UsbIo::PD::VENDOR_ID  = 0x0483;
const int UsbIo::PD::PRODUCT_ID = 0x5740;
const int UsbIo::PD::TIMEOUT    = 1000;

const int UsbIo::PD::EP_OUT = 0x03;
const int UsbIo::PD::EP_IN  = 0x81;

UsbIo::UsbIo()
{
    pd = new PD();
    pd->handle = 0;
    pd->timeout = PD::TIMEOUT;
    usb_init();
    usb_set_debug( 3 );
    pd->res.resize( 8 );
}

UsbIo::~UsbIo()
{
    if ( isOpen() )
        close();
    delete pd;
}

bool UsbIo::open( const std::string & arg )
{
    pd->handle = 0;
    int res = usbOpenDevice( &pd->handle, PD::VENDOR_ID, 0, PD::PRODUCT_ID, 0, 0, 0, 0 );
    bool result = ( (pd->handle != 0) && ( res == 0 ) );
	return result;
}

void UsbIo::close()
{
    if ( isOpen() )
    {
        usb_close( pd->handle );
        pd->handle = 0;
    }
}

bool UsbIo::isOpen() const
{
    return (pd->handle != 0);
}

int UsbIo::write( unsigned char * data, int size )
{
    int actual_length = usb_bulk_write( pd->handle,
                      PD::EP_OUT,
                      reinterpret_cast<char *>( data ), size,
                      pd->timeout );
    return actual_length;
}

int UsbIo::read( unsigned char * data, int maxSize )
{
    int actual_length = usb_bulk_read( pd->handle,
                      PD::EP_IN,
                      reinterpret_cast<char *>( data ), maxSize,
                      pd->timeout );
    return actual_length;
}

int UsbIo::setTimeout( int ms )
{
    pd->timeout = ms;
    return 0;
}

std::basic_string<unsigned char> & UsbIo::data()
{
    return pd->data;
}








