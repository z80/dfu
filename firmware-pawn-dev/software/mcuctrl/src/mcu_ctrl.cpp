
#include "mcu_ctrl.h"
#include <sstream>
#include <boost/regex.hpp>

#include <QtGui>

class Sleep: public QThread
{
public:
    static void msleep( int ms )
    {
        QThread::msleep( ms );
    }
};

McuCtrl::McuCtrl()
: UsbIo()
{

}

McuCtrl::~McuCtrl()
{

}

bool McuCtrl::flash( const std::string & fileName, std::string & result )
{
    // Read all data from file.
    std::basic_string<unsigned char> data;
    const int SZ = 1024;
    data.resize( SZ );
    int cnt = 0;
    FILE * fp = fopen( fileName.c_str(), "r" );
    do
    {
        if ( !fp )
            return false;
        int flashPage = 0;
        cnt = fread( const_cast<unsigned char *>( data.data() ), 1, SZ, fp );
        for ( int i=0; i<cnt; i++ )
        {
            std::ostringstream out;
            out << "pawnSetMem ";
            out << i;
            out << " ";
            out << static_cast<int>( data[i] );
            out << "\r\n";
            qDebug() << out.str().c_str();
            this->write( out.str() );
            Sleep::msleep( 30 );
        }
        std::ostringstream out;
        out << "pawnWriteFlash ";
        out << flashPage++;
        out << "\r\n";
        qDebug() << out.str().c_str();
        this->write( out.str() );
        Sleep::msleep( 30 );

        // Read back result.
        // ......

    }
    while ( cnt >= SZ );

    fclose( fp );

    return true;
}

/*
bool McuCtrl::inputs( unsigned long * data, int len )
{
    std::string stri = "st\r\n";
    int cntWr = write( stri );
    int cntRd = read( stri );
    // Here it should be output parser.
    std::string::const_iterator start = stri.begin();
    std::string::const_iterator end   = stri.end();
    boost::regex patt( "\\{([\\w\\s]{1,})\\}" );
    boost::smatch match;
    if ( boost::regex_search( start, end, match, patt ) )
    {
        //std::cout << it->str() << "\n";
        std::string ss = match[1];
        std::istringstream in( ss );
        for ( int i=0; i<len; i++ )
            in >> data[i];
        return true;
    }
    return false;
}

bool McuCtrl::setOutputs( unsigned long * data, int len )
{
    std::ostringstream out;
    out << "out ";
    for ( int i=0; i<len; i++ )
    {
        out << data[i];
        if ( i < (len-1) )
            out << " ";
    }
    out << "\r\n";
    int cntWr = write( out.str() );
    std::string stri;
    stri.resize( 64 );
    int cntRd = read( stri );
    return ( cntRd >= cntWr );
}

bool McuCtrl::accInit()
{
    // Accelerometer.
    unsigned char accAddr = 25,
                  magAddr = 30;
    unsigned char data[6];

    // Init acc.
    bool res = i2cSetAddr( accAddr );
    if ( !res )
        return false;
    data[0] = 0x20;
    data[1] = 0b00100111; //0x27;
    res = i2cSetBuf( 0, data, 2 );
    if ( !res )
        return false;
    res = i2cIo( 2, 0 );
    if ( !res )
        return false;
    msleep( 100 );
    int status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    //res = c.i2cStatus( status );
    //res = c.i2cStatus( status );

    res = i2cSetAddr( accAddr );
    if ( !res )
        return false;
    data[0] = 0x23;
    data[1] = 0b00000000;
    res = i2cSetBuf( 0, data, 2 );
    if ( !res )
        return false;
    res = i2cIo( 2, 0 );
    if ( !res )
        return false;
    msleep( 100 );
    status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    //res = i2cStatus( status );
    //res = i2cStatus( status );

    // Init mag.
    res = i2cSetAddr( magAddr );
    if ( !res )
        return false;
    data[0] = 0x00;
    data[1] = 0b10010000; //0x00;
    res = i2cSetBuf( 0, data, 2 );
    if ( !res )
        return false;
    res = i2cIo( 2, 0 );
    if ( !res )
        return false;
    msleep( 100 );
    status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    //res = i2cStatus( status );
    //res = i2cStatus( status );

    data[0] = 0x02;
    data[1] = 0b00000000; //0x00;
    res = i2cSetBuf( 0, data, 2 );
    if ( !res )
        return false;
    res = i2cIo( 2, 0 );
    if ( !res )
        return false;
    msleep( 100 );
    status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    //res = i2cStatus( status );
    //res = i2cStatus( status );
    return true;
}

bool McuCtrl::accAcc( int & x, int & y, int & z )
{
    unsigned char accAddr = 25;
    unsigned char data[6];
    int g[3];

    bool res = i2cSetAddr( accAddr );
    if ( !res )
        return false;
    // Reading back.
    data[0] = (0x28 | (1<<7));
    res = i2cSetBuf( 0, data, 1 );
    if ( !res )
        return false;
    res = i2cIo( 1, 6 );
    if ( !res )
        return false;
    msleep( 50 );
    int status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    if ( status != 0 )
        return false;
    //res = i2cStatus( status );
    //res = i2cStatus( status );
    res = i2cBuffer( 6, data );
    if ( !res )
        return false;
    for ( int j=0; j<3; j++ )
    {
        g[j] = ( static_cast<int>(data[j*2]) | (static_cast<int>(data[j*2+1]) * 256) );
        if ( g[j] & (1<<15) )
            g[j] -= 65536;
    }
    x = g[0];
    y = g[1];
    z = g[2];
    return true;
}

bool McuCtrl::accMag( int & x, int & y, int & z )
{
    unsigned char magAddr = 30;
    unsigned char data[6];
    int g[3];

    bool res = i2cSetAddr( magAddr );
    if ( !res )
        return false;
    // Mag read.
    data[0] = 0x03;
    res = i2cSetBuf( 0, data, 1 );
    if ( !res )
        return false;
    res = i2cIo( 1, 6 );
    if ( !res )
        return false;
    msleep( 50 );
    int status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    if ( status != 0 )
        return false;
    //res = i2cStatus( status );
    //res = i2cStatus( status );
    res = i2cBuffer( 6, data );
    if ( !res )
        return false;
    for ( int j=0; j<3; j++ )
    {
        g[j] = (static_cast<int>(data[j*2]) * 256) | static_cast<int>(data[j*2+1]);
        if ( g[j] & (1<<15) )
            g[j] -= 65536;
    }
    // The order is x, z, y.
    x = g[0];
    z = g[1];
    y = g[2];
    return true;
}

bool McuCtrl::accTemp( int & t )
{
    unsigned char magAddr = 30;
    unsigned char data[6];

    bool res = i2cSetAddr( magAddr );
    if ( !res )
        return false;
    // Temp read.
    data[0] = 0x31;
    res = i2cSetBuf( 0, data, 1 );
    if ( !res )
        return false;
    res = i2cIo( 1, 2 );
    if ( !res )
        return false;
    msleep( 50 );
    int status = -1;
    res = i2cStatus( status );
    if ( !res )
        return false;
    if ( status != 0 )
        return false;
    //res = i2cStatus( status );
    //res = i2cStatus( status );
    res = i2cBuffer( 2, data );
    if ( !res )
        return false;
    t = static_cast<int>( data[0]) * 256 + static_cast<int>(data[1]);
    return true;
}


bool McuCtrl::i2cSetAddr( int addr )
{
    std::ostringstream out;
    out << "addr ";
    out << addr;
    out << "\r\n";
    int cntWr = write( out.str() );
    std::string stri;
    stri.resize( 64 );
    int cntRd = read( stri );
    return ( cntRd >= cntWr );
}

bool McuCtrl::i2cSetBuf( int start, unsigned char * data, int cnt )
{
    std::ostringstream out;
    out << "set_buf ";
    out << start;
    for ( int i=0; i<cnt; i++ )
    {
        out << " ";
        out << static_cast<int>( data[i] );
    }
    out << "\r\n";
    int cntWr = write( out.str() );
    std::string stri;
    stri.resize( 64 );
    int cntRd = read( stri );
    return ( cntRd >= cntWr );
}

bool McuCtrl::i2cIo( int txCnt, int rxCnt )
{
    std::ostringstream out;
    out << "io ";
    out << txCnt;
    out << " ";
    out << rxCnt;
    out << "\r\n";
    int cntWr = write( out.str() );
    std::string stri;
    stri.resize( 64 );
    int cntRd = read( stri );
    return ( cntRd >= cntWr );
}

bool McuCtrl::i2cStatus( int & status )
{
    std::ostringstream out;
    out << "status\r\n";
    int cntWr = write( out.str() );
    std::string stri;
    stri.resize( 64 );
    int cntRd = read( stri );
    std::string::const_iterator start = stri.begin();
    std::string::const_iterator end   = stri.end();
    boost::regex patt( "\\{([\\w\\s]{1,})\\}" );
    boost::smatch match;
    if ( boost::regex_search( start, end, match, patt ) )
    {
        std::string ss = match[1];
        std::istringstream in( ss );
        in >> status;
        return true;
    }
    return false;
}

bool McuCtrl::i2cBuffer( int cnt, unsigned char * data )
{
    std::ostringstream out;
    out << "buffer ";
    out << cnt;
    out << "\r\n";
    int cntWr = write( out.str() );
    std::string stri;
    stri.resize( 64 );
    int cntRd = read( stri );
    std::string::const_iterator start = stri.begin();
    std::string::const_iterator end   = stri.end();
    boost::regex patt( "\\{([\\w\\s]{1,})\\}" );
    boost::smatch match;
    if ( boost::regex_search( start, end, match, patt ) )
    {
        std::string ss = match[1];
        std::istringstream in( ss );
        for ( int i=0; i<cnt; i++ )
        {
            int v;
            in >> v;
            data[i] = static_cast<unsigned char>( v );
        }
        return true;
    }
    return false;
}
*/








