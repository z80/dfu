

#include "usb_io.h"
#include <string.h>
#include <libusb.h>

const int UsbIo::VENDOR_ID  = 0x0483;
const int UsbIo::PRODUCT_ID = 0x5740;
const int UsbIo::TIMEOUT    = 1000;

inline static bool LSB()
{
    int i = 1;
    unsigned char * p = reinterpret_cast<unsigned char *>( &i );
    bool res = (p[0] != 0);
    return res;
}

// Low level functions.
#define CMD_PUT_ARGS      0
#define CMD_EXEC_FUNC     1
#define CMD_IN_QUEUE_SIZE 2
#define CMD_GET_IN_QUEUE  3

class UsbIo::PD
{
public:
    PD() {}
    ~PD() {}
    libusb_device_handle  * handle;
    libusb_context        * cxt;
    std::string err;
    std::string res;
    std::string output;
    int timeout;
    std::basic_string<unsigned char> data;
};

UsbIo::UsbIo()
{
    pd = new PD();
    pd->handle = 0;
    pd->timeout = 500;
    int res = libusb_init( &pd->cxt );
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

bool UsbIo::open()
{
    libusb_device * * list;
    int sz = libusb_get_device_list( pd->cxt, &list );
    for ( int i=0; i<sz; i++ )
    {    
    }
    pd->handle = libusb_open_device_with_vid_pid( pd->cxt, VENDOR_ID, PRODUCT_ID );
    libusb_free_device_list( list, 1 );
    bool result = (pd->handle != 0);
    if ( !result )
        return false;
    libusb_device * dev = libusb_get_device( pd->handle );
    libusb_device_descriptor devDesc;
    int res = libusb_get_device_descriptor( dev, &devDesc );
    libusb_config_descriptor * confDesc;
    res = libusb_get_active_config_descriptor( dev, &confDesc );
    libusb_free_config_descriptor( confDesc );
    unsigned char buf[1024];
    libusb_endpoint_descriptor epDesc;
    res = libusb_get_descriptor( pd->handle, LIBUSB_DT_ENDPOINT, 0,
    		                     buf, sizeof(buf) );
    res = libusb_claim_interface( pd->handle, 0 );

    for ( int i=0; i<255; i++ )
    {
    	unsigned char data[64];
    	int actual_length;
    	res = libusb_bulk_transfer( pd->handle,
                 i,
                 data, sizeof(data),
                 &actual_length,
                 1000 );
    	if ( res >= 0 )
    		res ++;
    }
    /*res = usb_claim_interface( pd->handle, 0 );
    struct usb_interface_descriptor desc;
    res = usb_get_descriptor( pd->handle, USB_DT_INTERFACE, 0, &desc, sizeof(desc) );
    int i;
    //for ( int i=0; i<desc.bNumEndpoints; i++ )
    for ( int i=0; i<5; i++ )
    {
        struct usb_endpoint_descriptor ep;
        res = usb_get_descriptor( pd->handle, USB_DT_ENDPOINT, i, &ep, sizeof( ep ) );
        int addr = ep.bEndpointAddress;
        int attr = ep.bmAttributes;
        int k = attr;
    }*/

    return true;
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

int UsbIo::write( void * data, int size )
{
    for ( int i=0; i<15; i++ )
    {
         int actual_length;
         int res = libusb_bulk_transfer( pd->handle,
                      (i | LIBUSB_ENDPOINT_OUT), 
                      reinterpret_cast<unsigned char *>( data ), size,
                      &actual_length, 
                      pd->timeout );
        /*int res = usb_bulk_write( pd->handle, 
                                  i, 
                                  reinterpret_cast<char *>( data ), 
                                  size, 
                                  pd->timeout );*/
        if ( res >= 0 )
            return res;
    }
    return 0;
}

int UsbIo::read( void * data, int maxSize )
{
    /*int res = usb_bulk_read( pd->handle, 
                             0x81, 
                             reinterpret_cast<char *>( data ), 
                             maxSize, 
                             pd->timeout );
    return res;*/
    return 0;
}

int UsbIo::setTimeout( int ms )
{
    pd->timeout = ms;
    return 0;
}

int UsbIo::putArgs( int start, int size, void * data )
{
    pd->data.clear();
    int sz = size + 3;
    pd->data.resize( sz );
    pd->data[0] = CMD_PUT_ARGS;
    pd->data[1] = static_cast<unsigned char>( start );
    pd->data[2] = static_cast<unsigned char>( size );
    unsigned char * d = reinterpret_cast< unsigned char * >( data );
    int k = 3;
    for ( int i=0; i<size; i++ )
        pd->data[k++] = d[i];
    int res = write( reinterpret_cast<void *>( const_cast<unsigned char *>( pd->data.data() ) ), sz );
    return res;
}

int UsbIo::putString( int start, char * stri )
{
    int sz = strlen( stri );
    int res = putArgs( start, sz, reinterpret_cast<void *>( stri ) );
    return res;
}

int UsbIo::putUInt8( int start, unsigned char val )
{
    int res = putArgs( start, sizeof(unsigned char), &val );
    return res;
}

// Here it is necessary to revert numbers to make things compatible with target's bytes order.
int UsbIo::putUInt16( int start, unsigned short val )
{
    unsigned char * p = reinterpret_cast<unsigned char *>( &val );
    if ( LSB() )
    {
        unsigned char b[ sizeof(unsigned short) ];
        b[0] = p[1];
        b[1] = p[0];
        int res = putArgs( start, sizeof(unsigned short), b );
        return res;
    }
    else
    {
        int res = putArgs( start, sizeof(unsigned short), p );
        return res;
    }
}

int UsbIo::putUInt32( int start, unsigned long val )
{
    unsigned char * p = reinterpret_cast<unsigned char *>( &val );
    if ( LSB() )
    {
        unsigned char b[ sizeof(unsigned short) ];
        b[0] = p[3];
        b[1] = p[2];
        b[2] = p[1];
        b[3] = p[0];
        int res = putArgs( start, sizeof(unsigned short), b );
        return res;
    }
    else
    {
        int res = putArgs( start, sizeof(unsigned long), &val );
        return res;
    }
}

int UsbIo::execFunc( int index )
{
    pd->data.clear();
    int sz = sizeof(int) + 1;
    pd->data.resize( sz );
    pd->data[0] = CMD_EXEC_FUNC;
    unsigned char * p = reinterpret_cast<unsigned char *>( &index );
    if ( LSB() )
    {
        pd->data[1] = p[3];
        pd->data[2] = p[2];
        pd->data[3] = p[1];
        pd->data[4] = p[0];
    }
    else
    {   
        pd->data[1] = p[1];
        pd->data[2] = p[2];
        pd->data[3] = p[3];
        pd->data[4] = p[4];
    }
    int res = write( reinterpret_cast<void *>( const_cast<unsigned char *>( pd->data.data() ) ), pd->data.size() );
    return res;
}

int UsbIo::queueSize()
{
    pd->data.clear();
    int sz = 1;
    pd->data.resize( sz );
    pd->data[0] = CMD_IN_QUEUE_SIZE;
    int res = write( reinterpret_cast<void *>( const_cast<unsigned char *>( pd->data.data() ) ), pd->data.size() );
    return res;
}

int UsbIo::readQueue( void * data, int maxSize )
{
    pd->data.clear();
    int sz = 1;
    pd->data.resize( sz );
    pd->data[0] = CMD_GET_IN_QUEUE;
    int res = write( reinterpret_cast<void *>( const_cast<unsigned char *>( pd->data.data() ) ), pd->data.size() );
    if ( res != sz )
        return -1;
    res = read( data, maxSize );
    return res;
}

//int UsbIo::io( const std::string & send, std::string & receive )
//{
//    if ( isOpen() )
//    {
//        int totalCnt = 0;
//        pd->output.clear();
//        receive.clear();
//        receive.reserve( send.size() );
//        std::string s = send;
//        s.push_back( '\0' );
//        for ( unsigned i=0; i<s.size(); i++ )
//        {
//            int cnt = usb_control_msg( pd->handle,
//                                       USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_IN,
//                                       s[i], 
//                                       0, 0, 
//                                       const_cast<char *>( pd->res.data() ), pd->res.size(),
//                                       TIMEOUT );
//            if ( cnt < 0 )
//            {
//                pd->err = usb_strerror();
//                close();
//                open();
//                return -1;
//            }
//            else
//            {
//                for ( int j=0; j<cnt; j++ )
//                {
//                    receive.push_back( pd->res[j] );
//                    pd->output.push_back( pd->res[j] );
//                }
//                totalCnt += cnt;
//            }
//        }
//        return totalCnt;
//    }
//    else
//        return -1;
//}
//
//std::string UsbIo::lastCmd()
//{
//    std::string res = pd->output;
//    pd->output.clear();
//    return res;
//}
//

