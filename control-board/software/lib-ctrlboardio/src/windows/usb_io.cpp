

#include "usb_io.h"
#include <string.h>
extern "C"
{
    #include "libusb.h"
    //#include "opendevice.h"
}
#include <windows.h>

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
    libusb_context       * cxt;
    libusb_device_handle * handle;
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
    libusb_init( &pd->cxt );
    libusb_set_debug( pd->cxt, 3 );
    pd->res.resize( 8 );
}

UsbIo::~UsbIo()
{
    if ( isOpen() )
        close();
    libusb_exit( pd->cxt );
    delete pd;
}

bool UsbIo::open( const std::string & arg )
{
    pd->handle = 0;
    libusb_device * * l = 0;
    int cnt = libusb_get_device_list( pd->cxt, &l );
    pd->handle = libusb_open_device_with_vid_pid( pd->cxt, PD::VENDOR_ID, PD::PRODUCT_ID );
    libusb_free_device_list( l, 1 );
    bool result = (pd->handle != 0);

    int res = libusb_set_configuration( pd->handle, 1 );
    res = libusb_claim_interface( pd->handle, 1 );
	return result;
}

void UsbIo::close()
{
    if ( isOpen() )
    {
        libusb_close( pd->handle );
        pd->handle = 0;
    }
}

bool UsbIo::isOpen() const
{
    return (pd->handle != 0);
}

int UsbIo::write( unsigned char * data, int size )
{
    int actual_length;
    int res = libusb_bulk_transfer( pd->handle,
                      PD::EP_OUT, data, size,
                      &actual_length, pd->timeout );
    if ( res != LIBUSB_SUCCESS )
        return 0;
    return actual_length;
}

int UsbIo::read( unsigned char * data, int maxSize )
{
    int actual_length;
    int res = libusb_bulk_transfer( pd->handle,
                      PD::EP_IN, data, maxSize,
                      &actual_length, pd->timeout );
    if ( res != LIBUSB_SUCCESS )
        return 0;
    return actual_length;
}

int UsbIo::setTimeout( int ms )
{
    pd->timeout = ms;
    return 0;
}

int UsbIo::readQueue( unsigned char * data, int maxSize )
{
    int t = pd->timeout;
    int sz = maxSize;
    do {
        int len = read( data, sz );
        data += len;
        sz -= len;
        if ( len < 1 )
        {
            Sleep( 1 );
            t -= 1;
        }
    } while ( ( sz > 0 ) && ( t > 0 ) );
    return maxSize - sz;
}

std::basic_string<unsigned char> & UsbIo::data()
{
    return pd->data;
}








