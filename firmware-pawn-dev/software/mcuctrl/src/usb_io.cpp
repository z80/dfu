

#include "usb_io.h"
#include <string.h>
#include <libusb.h>

#ifdef WIN32
    #include <windows.h>
    #define  delay( arg ) Sleep( arg )
#else
    #include <unistd.h>
    #define delay( arg ) usleep( 1000 * (arg) )
#endif

class UsbIo::PD
{
public:
    PD() {}
    ~PD() {}

    void clearOutput();
    libusb_context       * cxt;
    libusb_device_handle * handle;
    int timeout;
    static const int VENDOR_ID;
    static const int PRODUCT_ID;
    static const int TIMEOUT;
    static const int EP_OUT;
    static const int EP_IN;
    static const int STRI_MIN_LEN;
};

const int UsbIo::PD::VENDOR_ID  = 0x0483;
const int UsbIo::PD::PRODUCT_ID = 0x5740;
const int UsbIo::PD::TIMEOUT    = 500;

const int UsbIo::PD::EP_OUT = 0x03;
const int UsbIo::PD::EP_IN  = 0x81;

const int UsbIo::PD::STRI_MIN_LEN = 64;

void UsbIo::PD::clearOutput()
{
    //char cmd[] = "mem\r\n";
    int actual_length, res;
    //res = libusb_bulk_transfer( handle,
    //              EP_OUT, reinterpret_cast<unsigned char *>( cmd ), sizeof(cmd),
    //              &actual_length, 1 );

	unsigned char data[ STRI_MIN_LEN ];
    do {
    	actual_length = 0;
        res = libusb_bulk_transfer( handle,
						  EP_IN, data, STRI_MIN_LEN,
						  &actual_length, 1 );
		if ( res != LIBUSB_SUCCESS )
			break;

   } while ( actual_length > 0 );
}

UsbIo::UsbIo()
{
    pd = new PD();
    pd->handle = 0;
    pd->timeout = PD::TIMEOUT;
    libusb_init( &pd->cxt );
    libusb_set_debug( pd->cxt, 3 );
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
    if ( !result )
    	return false;
    if ( libusb_kernel_driver_active( pd->handle, 1 ) )
    	libusb_detach_kernel_driver( pd->handle, 1 );

    //int res = libusb_set_configuration( pd->handle, 1 );
    //res = libusb_claim_interface( pd->handle, 1 );

    pd->clearOutput();
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

int UsbIo::write( const std::string & stri )
{
	pd->clearOutput();
    int actual_length;
    unsigned char * data = reinterpret_cast<unsigned char *>( const_cast<char *>( stri.data() ) );
    int res = libusb_bulk_transfer( pd->handle, 
                      PD::EP_OUT, data, stri.size(), 
                      &actual_length, pd->timeout );
    if ( res != LIBUSB_SUCCESS )
        return res;
    return actual_length;
}

int UsbIo::read( std::string & stri )
{
	int len = 0;
	int timeout = pd->timeout;
	if ( stri.size() < PD::STRI_MIN_LEN )
		stri.resize( PD::STRI_MIN_LEN );
	while ( ( timeout > 0 ) && ( len < stri.size() ) )
	{
		unsigned char * data = reinterpret_cast<unsigned char *>( const_cast<char *>( stri.data() ) + len );
		int actual_length;
		int res = libusb_bulk_transfer( pd->handle,
						  PD::EP_IN, data, stri.size(), 
						  &actual_length, pd->timeout );
		if ( res != LIBUSB_SUCCESS )
		{
			if ( len > 0 )
			{
				stri.resize( len );
				return len;
			}
			else
				return res;
		}
		len += actual_length;
        //if ( ( len > 0) && ( stri.at( len-1 ) == '\n' ) )
        if ( ( len > 0 ) && ( stri.find( "<\r\n", 0 ) != std::string::npos ) )
			break;
        timeout--;
        if ( timeout <= 0 )
        {
            len = static_cast<int>( TIMEOUT );
            break;
        }
        delay( 1 );
	}
	stri.resize( len );
    return len;
}

int UsbIo::setTimeout( int ms )
{
    pd->timeout = ms;
    return 0;
}

void UsbIo::msleep( int ms )
{
    delay( ms );
}







