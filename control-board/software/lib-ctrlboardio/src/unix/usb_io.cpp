

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
    int handle;
    std::string err;
    std::string res;
    std::string output;
    int timeout;
    std::basic_string<unsigned char> data;
    static const int TIMEOUT;
};

const int UsbIo::PD::TIMEOUT    = 1000;

UsbIo::UsbIo()
{
    pd = new PD();
    pd->handle = 0;
    pd->timeout = PD::TIMEOUT;
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
    std::string openPort;
    if ( arg.size() < 1 )
        openPort = "/dev/ttyACM0";
    else
        openPort = arg;
    pd->handle = ::open( openPort.data(), O_RDWR | O_NOCTTY | O_NDELAY );
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
    	::close( pd->handle );
        pd->handle = 0;
    }
}

bool UsbIo::isOpen() const
{
    return (pd->handle > 0);
}

int UsbIo::write( unsigned char * data, int size )
{
    int cnt = ::write( pd->handle, data, size );
    return cnt;
}

int UsbIo::read( unsigned char * data, int maxSize )
{
    int cnt = ::read( pd->handle, data, maxSize );
    return cnt;
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






