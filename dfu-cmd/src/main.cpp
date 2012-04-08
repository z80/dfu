
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

// DFU related includes.
#include <usb.h>
extern "C"
{
    #include "dfu.h"
    #include "stm32mem.h"
    #include "hexfile.h"
}

class Echo
{
public:
    Echo() {}
    ~Echo() {}

    const Echo & operator<<( const std::string & stri )
    {
        std::cout << stri << std::endl;
        return *this;
    }
};

Echo                       echo;

class TFile
{
public:
    std::string                mode;
    uint32_t                   addr;
    std::basic_string<uint8_t> data;
    struct usb_device *        usbDev;
    uint8_t                    usbConfig, 
                               usbIface;
    dfu_device *               dfuDev;
    TFile()
    {
        usbDev    = 0;
        usbConfig = 0;
        usbIface  = 0;

        dfuDev    = 0;
    }
    ~TFile()
    {
    }
};

static void usage( void );
static bool getReadWrite( int argc, char * argv[], TFile & f ); // true if "read" otherwise "false";
static bool getOptsHex( int argc, char * argv[], TFile & f );
static bool getOptsBin( int argc, char * argv[], TFile & f );
static void hexParser( void * context, uint32_t addr, const uint8_t * data, int len );
static bool extension( const std::string & stri, const std::string & ext );
static bool listDevices( void );
static bool deviceInfo( void );

int main( int argc, char ** argv )
{
    if ( argc < 2 )
    {
        usage();
        return 0;
    }
    TFile f;
    bool res = getReadWrite( argc, argv, f );
    if ( !res )
    {
        echo << "Wrong arguments number";
        return -1;
    }

    usb_init();
    if ( f.mode == "write" )
    {
        if ( !getOptsHex( argc, argv, f ) )
        {
            if ( !getOptsBin( argc, argv, f ) )
            {
                echo << "Unknown file format";
                return 1;
            }
        }
        // Write data to device.
        // .....
    }
    else if ( f.mode == "read" )
    {
    }
    else if ( f.mode == "list" )
    {
        if ( !listDevices() )
            return -1;
    }
    else if ( f.mode == "info" )
    {
        if ( !deviceInfo() )
            return -1;
    }
    return 0;
}

static void usage( void )
{
    echo << "Usage: ";
    echo << "List DFU devices:      dfu-cmd list";
    echo << "Info about DFU device: dfu-cmd info";
    echo << "Read FLASH content:    dfu-cmd read  <filename> <addr-from> <size-to-read>";
    echo << "In IntelHEX format:    dfu-cmd write <filename>.hex";
    echo << "In binary format:      dfu-cmd write <filename>.bin <start addr hex>";
}

static bool getReadWrite( int argc, char * argv[], TFile & f )
{
    if ( argc < 2 )
        return false;
    std::string stri = argv[1];
    std::transform( stri.begin(), stri.end(), stri.begin(), ::tolower );
    f.mode = stri;
    return true;
}

static bool getOptsHex( int argc, char * argv[], TFile & f )
{
    // Parse IntelHex and return begin address and data. 
    if ( argc < 3 )
        return false;
    std::string fname = argv[2];
    if ( !extension( fname, ".hex" ) )
        return false;
    int res = hexfile_parse( fname.data(), hexParser, reinterpret_cast<void *>( &f ) );
    if ( res )
        return false;
    std::ostringstream st;
    st << "Address: " << std::hex << f.addr << ", size: " << f.data.size();
    echo << st.str();

    return true;
}

static bool getOptsBin( int argc, char * argv[], TFile & f )
{
    // Parse IntelHex and return begin address and data. 
    if ( argc < 4 )
        return false;
    std::string fname = argv[2];
    if ( !extension( fname, ".bin" ) )
        return false;
    std::string addr = argv[3];
    std::istringstream ist( addr );
    ist >> std::hex >> f.addr;

    FILE * fp = fopen( fname.data(), "rb" );
    if ( !fp )
    {
        std::ostringstream os;
        os << "Error: file \"" << fname << "\" not found";
        echo << os.str();
        return false;
    }

    fseek( fp , 0 , SEEK_END );
    size_t sz = ftell( fp );
    rewind( fp );
    f.data.resize( sz );
    size_t res = fread( reinterpret_cast<void *>( const_cast<uint8_t *>( f.data.data() ) ), 1, sz, fp );
    if ( res != sz )
    {
        std::ostringstream os;
        os << "Error: file can\'t be read";
        echo << os.str();
        return false;
    }
    fclose( fp );
    std::ostringstream st;
    st << "Address: " << f.addr << ", size: " << f.data.size();
    echo << st.str();


    return true;
}

static void hexParser( void * context, uint32_t addr, const uint8_t * data, int len )
{
    TFile * f = reinterpret_cast<TFile *>( context );
    f->addr = ( f->addr < addr ) ? f->addr : addr;
    for ( int i=0; i<len; i++ )
        f->data.push_back( data[i] );
}

static bool extension( const std::string & stri, const std::string & ext )
{
    std::string e = stri.substr( std::max<size_t>( stri.size() - ext.length(), 0 ) );
    std::transform( e.begin(), e.end(), e.begin(), ::tolower );
    if ( e != ext )
        return false;
    return true;
}

static  void dfuEnumCb( void * context, struct usb_device * dev, 
                        uint8_t config, uint16_t iface );

static bool listDevices( void )
{
    echo << "Dfu devices list:";
    dfu_find_devs( 0, dfuEnumCb );
    return true;
}

static  void dfuEnumCb( void * context, struct usb_device * dev, 
                        uint8_t config, uint16_t iface )
{
    std::ostringstream st;
    st << "Config: " << config << ", interface: " << iface;
    echo << st.str();
    if ( context )
    {
        TFile * f = reinterpret_cast<TFile *>( context );
        if ( !f->usbDev )
        {
            f->usbDev = dev;
            f->usbConfig = config;
            f->usbIface  = iface;
        }
    }
}

static bool deviceInfo( void )
{
    TFile f;
    dfu_find_devs( &f, dfuEnumCb );
    if ( !f.usbDev )
    {
        echo << "Error: No DFU devices found";
        return false;
    }
    
    f.dfuDev = dfu_open( f.usbDev, f.usbConfig, f.usbIface );
    if ( !f.dfuDev )
    {
        echo << "Error: DFU device open failed";
        return false;
    }
    mem_target info;
    const int id = 0; // !!! Don't know what it is yet.
    int res = stm32mem_get_target( f.dfuDev, f.usbIface, &info );
    if ( res )
    {
        echo << "Error: failed to get FLASH info";
        return false;
    }
    std::ostringstream os;
    os << "Id: " << info.id;
    echo << os.str();
    os.clear();

    os << "Name: " << info.name;
    echo << os.str();
    os.clear();

    os << "Base: " << std::hex << info.base;
    echo << os.str();
    os.clear();

    os << "Sectors: " << info.num_sectors;
    echo << os.str();
    os.clear();

    for ( int i=0; i<info.num_sectors; i++ )
    {
        mem_sector sect;
        int res = stm32mem_get_sector( f.dfuDev, f.usbIface, i, &sect );
        if ( res )
        {
            echo << "Error: failed to get sector info";
            return false;
        }
        std::ostringstream os;
        os << "Id: " << sect.id;
        echo << os.str();
        os.clear();

        os << "Addr: " << std::hex << sect.addr;
        echo << os.str();
        os.clear();

        os << "Size: " << sect.size;
        echo << os.str();
        os.clear();

        std::string mode;
        if ( sect.mode == 1 )
            mode = "READ";
        else if ( sect.mode == 2 )
            mode = "WRITE";
        else if ( sect.mode == 4 )
            mode = "ERASE";
        else
            mode = "UNKNOWN";
        os << "Mode: " << mode;
        echo << os.str();
        os.clear();
    }

    return true;
}


