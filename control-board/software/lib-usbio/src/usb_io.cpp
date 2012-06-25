

#include "usb_io.h"
#include <string.h>
#include <libusb.h>

#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */

inline static bool LSB()
{
    int i = 1;
    unsigned char * p = reinterpret_cast<unsigned char *>( &i );
    bool res = (p[0] != 0);
    return res;
}

// Low level functions.
#define CMD_DATA    0
#define CMD_FUNC    1

class UsbIo::PD
{
public:
    PD() {}
    ~PD() {}
    //libusb_device_handle  * handle;
    //libusb_context        * cxt;
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
    int handle;
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
    //int res = libusb_init( &pd->cxt );
    //libusb_set_debug( pd->cxt, 3 );
    pd->res.resize( 8 );
}

UsbIo::~UsbIo()
{
    if ( isOpen() )
        close();
    //libusb_exit( pd->cxt );
    delete pd;
}

bool UsbIo::open()
{
    /*libusb_device * * list;
    int sz = libusb_get_device_list( pd->cxt, &list );
    for ( int i=0; i<sz; i++ )
    {    
    }
    pd->handle = libusb_open_device_with_vid_pid( pd->cxt, PD::VENDOR_ID, PD::PRODUCT_ID );
    libusb_free_device_list( list, 1 );
    bool result = (pd->handle != 0);
    if ( !result )
        return false;
    int res = libusb_kernel_driver_active( pd->handle, 0 );
    if ( res == 1 )
        res = libusb_detach_kernel_driver( pd->handle, 0 );

	return result;*/

	pd->handle = ::open( "/dev/ttyACM0", O_RDWR | O_NOCTTY | O_NDELAY );
	bool res = ( pd->handle > 0 );
	if ( res )
	{
		// Blocking if no characters.
	    fcntl( pd->handle, F_SETFL, 0 );
	    // Nonblocking in the case of no characters.
	    //fcntl( pd->handle, F_SETFL, FNDELAY );
	}
    return res;
}

void UsbIo::close()
{
    if ( isOpen() )
    {
        //libusb_close( pd->handle );
    	::close( pd->handle );
        pd->handle = 0;
    }
}

bool UsbIo::isOpen() const
{
    return (pd->handle != 0);
}

int UsbIo::write( unsigned char * data, int size )
{
    /*int actual_length;
    int res = libusb_bulk_transfer( pd->handle,
                      PD::EP_OUT,
                      data, size,
                      &actual_length, 
                      pd->timeout );
    if ( res == 0 )
        return actual_length;
    return 0;*/

	int cnt = ::write( pd->handle, data, size );
	return cnt;
}

int UsbIo::read( unsigned char * data, int maxSize )
{
    /*int actual_length;
    int res = libusb_bulk_transfer( pd->handle,
                      PD::EP_IN,
                      data, maxSize,
                      &actual_length,
                      pd->timeout );
    if ( res == 0 )
        return actual_length;
    return 0;*/

	int cnt = ::read( pd->handle, data, maxSize );
	return cnt;
}

int UsbIo::setTimeout( int ms )
{
    pd->timeout = ms;
    return 0;
}

int UsbIo::putArgs( int size, unsigned char * data )
{
    pd->data.clear();
    int sz = size + 2;
    pd->data.resize( sz );
    pd->data[0] = CMD_DATA;
    pd->data[1] = static_cast<unsigned char>( size );
    unsigned char * d = data;
    int k = 2;
    for ( int i=0; i<size; i++ )
        pd->data[k++] = d[i];
    int res = write( const_cast<unsigned char *>( pd->data.data() ), sz );
    return res;
}

int UsbIo::putString( char * stri )
{
    int sz = strlen( stri );
    int res = putArgs( sz, reinterpret_cast<unsigned char *>( stri ) );
    return res;
}

int UsbIo::putUInt8( unsigned char val )
{
    int res = putArgs( sizeof(unsigned char), &val );
    return res;
}

// Here it is necessary to revert numbers to make things compatible with target's bytes order.
int UsbIo::putUInt16( unsigned short val )
{
    unsigned char * p = reinterpret_cast<unsigned char *>( &val );
    if ( LSB() )
    {
        unsigned char b[ sizeof(unsigned short) ];
        b[0] = p[1];
        b[1] = p[0];
        int res = putArgs( sizeof(unsigned short), b );
        return res;
    }
    else
    {
        int res = putArgs( sizeof(unsigned short), p );
        return res;
    }
}

int UsbIo::putUInt32( unsigned long val )
{
    unsigned char * p = reinterpret_cast<unsigned char *>( &val );
    if ( LSB() )
    {
        unsigned char b[ sizeof(unsigned long) ];
        b[0] = p[3];
        b[1] = p[2];
        b[2] = p[1];
        b[3] = p[0];
        int res = putArgs( sizeof(unsigned long), b );
        return res;
    }
    else
    {
        int res = putArgs( sizeof(unsigned long), p );
        return res;
    }
}

int UsbIo::execFunc( int index )
{
    pd->data.clear();
    int sz = 2;
    pd->data.resize( sz );
    pd->data[0] = CMD_FUNC;
    pd->data[1] = static_cast<unsigned char>( index );
    int res = write( const_cast<unsigned char *>( pd->data.data() ), pd->data.size() );
    return res;
}

/*int UsbIo::queueSize()
{
    pd->data.clear();
    int sz = 1;
    pd->data.resize( sz );
    pd->data[0] = CMD_IN_QUEUE_SIZE;
    int res = write( const_cast<unsigned char *>( pd->data.data() ), pd->data.size() );
    return res;
}*/

int UsbIo::readQueue( unsigned char * data, int maxSize )
{
    int res;
    int sz = maxSize;
    int cnt = 0;
    unsigned char * d = data;
    do {
        res = read( d, sz );
        cnt += res;
        d += res;
        sz -= res;
    } while ( ( res > 0 ) && ( sz > 0 ) );
    return cnt;
}







