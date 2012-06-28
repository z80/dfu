
#include "luabayspec.h"
#include "bayspec_cam.h"

static const char META[] = "LUA_BAYSPEC_META";
static const char LIB_NAME[] = "luabayspec";

static int create( lua_State * L )
{
    int cnt = lua_gettop( L );
    CAndorBase * ab = new BayspecCam();
    CAndorBase * * p = reinterpret_cast< CAndorBase * * >( lua_newuserdata( L, sizeof( CAndorBase * ) ) );
    *p = dynamic_cast<CAndorBase *>( ab );
    luaL_getmetatable( L, META );
    lua_setmetatable( L, -2 );
    return 1;
}

static int gc( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    if ( ab )
        ab->deleteLater();
    return 0;
}

static int self( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    lua_pushlightuserdata( L, reinterpret_cast< void * >(ab) );
    return 1;
}

static int mono( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    BayspecCam * bs = dynamic_cast<BayspecCam *>( ab );
    if ( !bs )
    {
        lua_pushnil( L );
        return 1;
    }
    AbstractSpgrControl * amono = dynamic_cast<AbstractSpgrControl *>( bs->mono() );
    lua_pushlightuserdata( L, reinterpret_cast< void * >( amono ) );
    return 1;
}

static int calibration( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    BayspecCam * bs = dynamic_cast<BayspecCam *>( ab );
    if ( !bs )
    {
        lua_pushnil( L );
        return 1;
    }
    AbstractSpgrControl * amono = bs->mono();
    QVector<float> res = amono->calibration( 0, 0, 0 );

    lua_newtable( L );
    int ind = 1;
    for ( QVector<float>::const_iterator i=res.begin(); i!=res.end(); i++ )
    {
        lua_pushnumber( L, static_cast<lua_Number>( ind ) );
        lua_pushnumber( L, static_cast<lua_Number>( *i ) );
        ind ++;
        lua_settable( L, -3 );
    }
    return 1;
}

static int version( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    std::string res = ab->dllVersion().toStdString();
    lua_pushstring( L, res.data() );
    return 1;
}

static int libIsInit( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    bool res = ab->libIsInit();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int cameras( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int cnt = ab->cameras();
    lua_pushnumber( L, static_cast<lua_Number>( cnt ) );
    return 1;
}

static int camera( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int res = ab->camera();
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static  int setCamera( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int index = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setCamera( index );
    return 0;
}


// **************************************************************************
//                Функции работы с камерой.
// **************************************************************************
static int cameraHandle( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int index = static_cast<int>( lua_tonumber( L, 2 ) );
    int handle = ab->cameraHandle( index );
    lua_pushnumber( L, static_cast<lua_Number>( handle ) );
    return 1;
}

static int init( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int args = lua_gettop( L );
    if ( args > 1 )
    {
        QString path = lua_tostring( L, 2 );
        ab->init( path );
    }
    else
        ab->init();
    return 0;
}

static int isInit( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    bool res = ab->isInit();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int lastError( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    QString res = ab->lastError();
    if ( !res.isNull() )
        lua_pushstring( L, res.toAscii().data() );
    else
        lua_pushnil( L );
    return 1;
}

static int temperatureRange( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    qreal tmin, tmax;
    ab->temperatureRange( tmin, tmax );
    lua_pushnumber( L, static_cast<lua_Number>( tmin ) );
    lua_pushnumber( L, static_cast<lua_Number>( tmax ) );
    return 2;
}

static int temperature( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    lua_pushnumber( L, static_cast<lua_Number>( ab->temperature() ) );
    return 1;
}

static int setTemperatureTarget( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int arg = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setTemperatureTarget( arg );
    return 0;
}

static int isTemperatureStabilized( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    bool res = ab->isTemperatureStabilized();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int coolerOn( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->coolerOn();
    return 0;
}

static int coolerOff( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->coolerOff();
    return 0;
}

static int startAcquisition( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->startAcquisition();
    return 0;
}

static int abortAcquisition( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->abortAcquisition();
    return 0;
}

static int isIdle( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    bool res = ab->isIdle();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int returnData( lua_State * L, const CSpecDataItem & d )
{
    if ( d.hasSpectrum( "counts" ) )
    {
        const QVector<qreal> & res = d.spectrum( "counts" );
        lua_newtable( L );
        
        int ind = 1;
        for ( QVector<qreal>::const_iterator i=res.begin(); i!=res.end(); i++ )
        {
            lua_pushnumber( L, static_cast<lua_Number>( ind ) );
            lua_pushnumber( L, static_cast<lua_Number>( *i ) );
            lua_settable(L, -3 );
            ind ++;
        }
        return 1;
    }
    return 0;
}

static int newestData( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    CSpecDataItem d = ab->newestData();
    return returnData( L, d );
}

static int newestDataToSpecData( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    CSpecDataItem * d = reinterpret_cast<CSpecDataItem *>( const_cast<void *>( lua_topointer( L, 2 ) ) );
    *d = ab->newestData();
    return 0;
}

static int oldestData( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    CSpecDataItem d = ab->oldestData();
    return returnData( L, d );
}

static int oldestDataToSpecData( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    CSpecDataItem * d = reinterpret_cast<CSpecDataItem *>( const_cast<void *>( lua_topointer( L, 2 ) ) );
    *d = ab->oldestData();
    return 0;
}

static int acquiredData( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    QVector<long> d = ab->acquiredData();
    lua_newtable( L );
    for ( int i=0; i<d.size(); i++ )
    {
        lua_pushnumber( L, static_cast<lua_Number>( i ) );
        lua_pushnumber( L, static_cast<lua_Number>( d.at( i ) ) );
        lua_settable( L, -3 );
    }
    return 1;
}

static int detectorSize( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    QSize sz = ab->detectorSize();
    lua_pushnumber( L, static_cast<lua_Number>( sz.width() ) );
    lua_pushnumber( L, static_cast<lua_Number>( sz.height() ) );
    return 2;
}

static int pixelSize( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    QSizeF sz = ab->pixelSize();
    lua_pushnumber( L, static_cast<lua_Number>( sz.width() ) );
    lua_pushnumber( L, static_cast<lua_Number>( sz.height() ) );
    return 2;
}

static int emGain( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int gain = ab->emGain();
    lua_pushnumber( L, static_cast<lua_Number>( gain ) );
    return 1;
}

static int setEmGain( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int gain = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setEmGain( gain );
    return 0;
}

static int emGainRange( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int gainMin = -1, gainMax = -1;
    ab->emGainRange( gainMin, gainMax );
    lua_pushnumber( L, static_cast<lua_Number>( gainMin ) );
    lua_pushnumber( L, static_cast<lua_Number>( gainMax ) );
    return 2;
}

static int setFan( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int mode = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setFan( mode );
    return 0;
}

static int fan( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int mode = ab->fan();
    lua_pushnumber( L, static_cast<qreal>( mode ) );
    return 1;
}

static int setTriggerMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int mode = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setTriggerMode( mode );
    return 0;
}

static int setFastExtTrigger( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int mode = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setFastExtTrigger( mode );
    return 0;
}

static int setAcquisitionMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int mode = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setAcquisitionMode( mode );
    return 0;
}

static int setReadMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int mode = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setReadMode( mode );
    return 0;
}

static int setShutter( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int ttlMode     = static_cast<int>( lua_tonumber( L, 2 ) );
    int openMode    = static_cast<int>( lua_tonumber( L, 3 ) );
    int openTimeMs  = static_cast<int>( lua_tonumber( L, 4 ) );
    int closeTimeMs = static_cast<int>( lua_tonumber( L, 5 ) );
    ab->setShutter( ttlMode, openMode, openTimeMs, closeTimeMs );
    return 0;
}

static int setImage( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int hBinn  = static_cast<int>( lua_tonumber( L, 2 ) );
    int vBinn  = static_cast<int>( lua_tonumber( L, 3 ) );
    int hStart = static_cast<int>( lua_tonumber( L, 4 ) );
    int hEnd   = static_cast<int>( lua_tonumber( L, 5 ) );
    int vStart = static_cast<int>( lua_tonumber( L, 6 ) );
    int vEnd   = static_cast<int>( lua_tonumber( L, 7 ) );
    ab->setImage( hBinn, vBinn, hStart, hEnd, vStart, vEnd );
    return 0;
}

static int setExposureTime( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    qreal t = static_cast<qreal>( lua_tonumber( L, 2 ) );
    ab->setExposureTime( t );
    return 0;
}

static int exposureTime( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    qreal res = ab->exposureTime();
    lua_pushnumber( L, static_cast<lua_Number>(res) );
    return 1;
}

static int setAccumulationsCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int cnt = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setAccumulationsCnt( cnt );
    return 0;
}

static int setAccumulationCycleTime( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    qreal time = static_cast<qreal>( lua_tonumber( L, 2 ) );
    ab->setAccumulationCycleTime( time );
    return 0;
}

static int setKineticsCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int cnt = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setKineticsCnt( cnt );
    return 0;
}

static int setKineticCycleTime( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    qreal time = static_cast<qreal>( lua_tonumber( L, 2 ) );
    ab->setKineticCycleTime( time );
    return 0;
}

static int acquisitionTimes( lua_State * L )
{
    return 0;
}

static int circularBufferSize( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int sz = ab->circularBufferSize();
    lua_pushnumber( L, static_cast<lua_Number>( sz ) );
    return 1;
}

static int acquiredImagesCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int start, end;
    int cnt = ab->acquiredImagesCnt( start, end );
    lua_pushnumber( L, static_cast<lua_Number>( cnt ) );
    lua_pushnumber( L, static_cast<lua_Number>( start ) );
    lua_pushnumber( L, static_cast<lua_Number>( end ) );
    return 3;
}

static int setFvbMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->setFvbMode();
    return 0;
}

static int setSingleTrackMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int center = static_cast<int>( lua_tonumber( L, 2 ) );
    int height = static_cast<int>( lua_tonumber( L, 3 ) );
    ab->setSingleTrackMode( center, height );
    return 0;
}

static int setImageMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->setImageMode();
    return 0;
}

static int setVsSpeed( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int index = static_cast<int>( lua_tonumber( L, 2 ) );
    ab->setVsSpeed( index );
    return 0;
}

static int vsSpeed( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int index = static_cast<int>( lua_tonumber( L, 2 ) );
    qreal res = ab->vsSpeed( index );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int vsSpeedsCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int res = ab->vsSpeedsCnt();
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int setFastestRecommendedVsSpeed( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->setFastestRecommendedVsSpeed();
    return 0;
}

static int setHsSpeed( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int type  = static_cast<int>( lua_tonumber( L, 2 ) );
    int index = static_cast<int>( lua_tonumber( L, 3 ) );
    ab->setHsSpeed( type, index );
    return 0;
}

static int hsSpeed( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int channel = static_cast<int>( lua_tonumber( L, 2 ) );
    int type    = static_cast<int>( lua_tonumber( L, 3 ) );
    int index   = static_cast<int>( lua_tonumber( L, 4 ) );
    qreal res = ab->hsSpeed( channel, type, index );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int hsSpeedsCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int channel = static_cast<int>( lua_tonumber( L, 2 ) );
    int type    = static_cast<int>( lua_tonumber( L, 3 ) );
    int res = ab->hsSpeedsCnt( channel, type );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int channels( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int res = ab->channels();
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int setChannel( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int channel = lua_tonumber( L, 2 );
    ab->setChannel( channel );
    return 0;
}

static int amplifierTypes( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int res = ab->amplifierTypes();
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int setAmplifierType( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    int type = lua_tonumber( L, 2 );
    ab->setAmplifierType( type );
    return 0;
}

static int isReadyShutdown( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    bool res = ab->isReadyShutdown();
    lua_pushnumber( L, ( res ) ? 1 : 0 );
    return 1;
}

static int shutdown( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    ab->shutdown();
    return 0;
}

static int setSensorMode( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    if ( lua_gettop( L ) > 1 )
    {
        BayspecCam * c = dynamic_cast<BayspecCam *>(ab);
        if ( !c )
        {
            lua_pushnil( L );
            return 1;
        }
        std::string stri = lua_tostring( L, 2 );
        bool res;
        if ( stri == "sensitive" )
            res = c->setSensorMode( BayspecCam::THighSensitive );
        else
            res = c->setSensorMode( BayspecCam::THighDynamicRange );
        lua_pushboolean( L, res ? 1 : 0 );
        return 1;
    }
    lua_pushnil( L );
    return 1;
}


static int spectrumRaw( lua_State * L )
{
    if ( lua_gettop( L ) >= 1 )
    {
        CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
        BayspecCam * c = dynamic_cast<BayspecCam *>(ab);
        if ( !c )
        {
            lua_pushnil( L );
            lua_pushstring( L, "ERROR: This is not Bayspec device!" );
            return 2;
        }
        c->spectrumRaw();
        lua_pushboolean( L, 1 );
        return 1;
    }
    lua_pushnil( L );
    lua_pushstring( L, "ERROR: no self pointer!" );
    return 2;
}

static int setMeasuresCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    BayspecCam * c = dynamic_cast<BayspecCam *>(ab);
    if ( !c )
    {
        lua_pushstring( L, "ERROR: This is not Bayspec device!" );
        return 1;
    }
    int cnt = static_cast<int>( lua_tonumber( L, 2 ) );
    c->setMeasuresCnt( cnt );
    return 0;
}

static int measuresCnt( lua_State * L )
{
    CAndorBase * ab = *reinterpret_cast<CAndorBase * *>( lua_touserdata( L, 1 ) );
    BayspecCam * c = dynamic_cast<BayspecCam *>(ab);
    if ( !c )
    {
        lua_pushnil( L );
        lua_pushstring( L, "ERROR: This is not Bayspec device!" );
        return 2;
    }
    int cnt = c->measuresCnt();
    lua_pushnumber( L, static_cast<lua_Number>( cnt ) );
    return 1;
}


// **************************************************************************
//                Регистрация функций в макросе.
// **************************************************************************

static const struct luaL_reg META_FUNCTIONS[] = {
	{ "__gc",                    gc }, 
    { "pointer",                 self }, 
    { "mono",                    mono }, 
    { "calibration",             calibration }, 
    { "version",                 version }, 
    
    { "libIsInit",               libIsInit }, 

    { "cameras",                 cameras }, 
    { "camera",                  camera }, 
    { "setCamera",               setCamera }, 

    { "cameraHandle",            cameraHandle }, 
    { "init",                    init }, 
    { "isInit",                  isInit }, 
    { "lastError",               lastError }, 
    { "temperatureRange",        temperatureRange }, 
    { "temperature",             temperature }, 
    { "setTemperatureTarget",    setTemperatureTarget }, 
    { "isTemperatureStabilized", isTemperatureStabilized }, 
    { "coolerOn",                coolerOn }, 
    { "coolerOff",               coolerOff }, 
    { "startAcquisition",        startAcquisition }, 
    { "abortAcquisition",        abortAcquisition }, 
    { "isIdle",                  isIdle }, 
    { "newestData",              newestData }, 
    { "newestDataToSpecData",    newestDataToSpecData }, 
    { "oldestData",              oldestData }, 
    { "oldestDataToSpecData",    oldestDataToSpecData }, 
    { "acquiredData",            acquiredData }, 
    
    { "detectorSize",            detectorSize }, 
    { "pixelSize",               pixelSize }, 
    { "emGain",                  emGain }, 
    { "setEmGain",               setEmGain }, 
    { "emGainRange",             emGainRange }, 
    { "setFan",                  setFan }, 
    { "fan",                     fan }, 
    { "setTriggerMode",          setTriggerMode }, 
    { "setFastExtTrigger",       setFastExtTrigger }, 
    
    { "setAcquisitionMode",      setAcquisitionMode }, 
    { "setReadMode",             setReadMode }, 
    { "setShutter",              setShutter }, 
    { "setImage",                setImage }, 
    { "setExposureTime",         setExposureTime }, 
    { "exposureTime",            exposureTime }, 

    { "setAccumulationsCnt",     setAccumulationsCnt }, 
    { "setAccumulationCycleTime", setAccumulationCycleTime }, 
    { "setKineticsCnt",          setKineticsCnt }, 
    { "setKineticCycleTime",     setKineticCycleTime }, 
    { "acquisitionTimes",        acquisitionTimes }, 
    { "circularBufferSize",      circularBufferSize }, 
    { "acquiredImagesCnt",       acquiredImagesCnt }, 

    { "setFvbMode",              setFvbMode }, 
    { "setSingleTrackMode",      setSingleTrackMode }, 
    { "setImageMode",            setImageMode }, 
    { "setVsSpeed",              setVsSpeed }, 
    { "vsSpeed",                 vsSpeed }, 
    { "vsSpeedsCnt",             vsSpeedsCnt }, 
    { "setFastestRecommendedVsSpeed", setFastestRecommendedVsSpeed }, 
    { "setHsSpeed",              setHsSpeed }, 
    { "hsSpeed",                 hsSpeed }, 
    { "hsSpeedsCnt",             hsSpeedsCnt }, 
    { "channels",                channels }, 
    { "setChannel",              setChannel }, 
    { "amplifierTypes",          amplifierTypes }, 
    { "setAmplifierType",        setAmplifierType }, 
    { "isReadyShutdown",         isReadyShutdown }, 
    { "shutdown",                shutdown }, 

    { "setSensorMode",           setSensorMode }, 

    { "spectrumRaw",             spectrumRaw }, 
    { "setMeasuresCnt",          setMeasuresCnt }, 
    { "measuresCnt",             measuresCnt }, 

    { NULL,           NULL }, 
};

static void createMeta( lua_State * L )
{
    luaL_newmetatable( L, META );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );       // metatable.__index = metatable
    luaL_register( L, NULL, META_FUNCTIONS );
    // Очищаем стек.
    lua_pop( L, lua_gettop( L ) );
}

static const struct luaL_reg FUNCTIONS[] = {
	{ "create",  create }, 
	{ NULL, NULL },
};

static void registerFunctions( lua_State * L )
{
    luaL_register( L, LIB_NAME, FUNCTIONS );
}

extern "C" int __declspec(dllexport) luaopen_luabayspec( lua_State * L )
{
    createMeta( L );
    registerFunctions( L );
    return 0;
}





