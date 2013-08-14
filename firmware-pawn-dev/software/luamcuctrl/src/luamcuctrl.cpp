
#include "luamcuctrl.h"
#include "mcu_ctrl.h"

static const char META[] = "LUA_MCUCTRL_META";
static const char LIB_NAME[] = "luamcuctrl";

static int create( lua_State * L )
{
    int cnt = lua_gettop( L );
    McuCtrl * io = new McuCtrl();
    McuCtrl * * p = reinterpret_cast< McuCtrl * * >( lua_newuserdata( L, sizeof( McuCtrl * ) ) );
    *p = dynamic_cast<McuCtrl *>( io );
    luaL_getmetatable( L, META );
    lua_setmetatable( L, -2 );
    return 1;
}

static int gc( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    if ( io )
        delete io;
    return 0;
}

static int self( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    lua_pushlightuserdata( L, reinterpret_cast< void * >(io) );
    return 1;
}

static int open( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    bool res;
    if ( ( lua_gettop( L ) > 1 ) && ( lua_type( L, 2 ) == LUA_TSTRING ) )
    {
        std::string arg = lua_tostring( L, 2 );
        res = io->open( arg );
    }
    else
        res = io->open();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int close( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    io->close();
    return 0;
}

static int isOpen( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    bool res = io->isOpen();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int write( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    std::string stri = lua_tostring( L, 2 );
    int res = io->write( stri );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int read( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    std::string stri;
    int res = io->read( stri );
    if ( res > 0 )
        lua_pushstring( L, stri.c_str() );
    else
        lua_pushnil( L );
    return 1;
}

static int setOutputs( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int cnt = lua_gettop( L );
    std::basic_string<unsigned long> args;
    args.resize( cnt - 1 );
    for ( int i=2; i<=cnt; i++ )
        args[i-2] = static_cast<unsigned long>( lua_tonumber( L, i ) );
    bool res = io->setOutputs( const_cast<unsigned long *>( args.data() ), static_cast<int>( args.size() ) );
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int inputs( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int cnt;
    if ( lua_gettop( L ) > 1 )
        cnt = static_cast<int>( lua_tonumber( L, 2 ) );
    else
        cnt = 3;
    std::basic_string<unsigned long> args;
    args.resize( cnt );
    bool res = io->inputs( const_cast<unsigned long *>( args.data() ), cnt );
    if ( res )
    {
        for ( int i=0; i<cnt; i++ )
            lua_pushnumber( L, static_cast<lua_Number>( args[i] ) );
        return cnt;
    }
    return 0;
}

static int accInit( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    bool res = io->accInit();
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int accAcc( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int x, y, z;
    bool res = io->accAcc( x, y, z );
    lua_pushboolean( L, res ? 1 : 0 );
    if ( !res )
        return 1;
    lua_pushnumber( L, static_cast<lua_Number>( x ) );
    lua_pushnumber( L, static_cast<lua_Number>( y ) );
    lua_pushnumber( L, static_cast<lua_Number>( z ) );
    return 4;
}

static int accMag( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int x, y, z;
    bool res = io->accMag( x, y, z );
    lua_pushboolean( L, res ? 1 : 0 );
    if ( !res )
        return 1;
    lua_pushnumber( L, static_cast<lua_Number>( x ) );
    lua_pushnumber( L, static_cast<lua_Number>( y ) );
    lua_pushnumber( L, static_cast<lua_Number>( z ) );
    return 4;
}

static int accTemp( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int t;
    bool res = io->accTemp( t );
    lua_pushboolean( L, res ? 1 : 0 );
    if ( !res )
        return 1;
    lua_pushnumber( L, static_cast<lua_Number>( t ) );
    return 2;
}

static int i2cSetAddr( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int addr = static_cast<int>( lua_tonumber( L, 2 ) );
    bool res = io->i2cSetAddr( addr );
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int i2cSetBuf( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int start = static_cast<int>( lua_tonumber( L, 2 ) );
    int cnt = lua_gettop( L );
    for ( int i=3; i<=cnt; i++ )
    {
        unsigned char val = static_cast<unsigned char>( lua_tonumber( L, i ) );
        bool res = io->i2cSetBuf( start, &val, 1 );
        start++;
        if ( !res )
        {
            lua_pushboolean( L, 0 );
            return 1;
        }
    }
    lua_pushboolean( L, 1 );
    return 1;
}

static int i2cIo( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int txCnt = static_cast<int>( lua_tonumber( L, 2 ) );
    int rxCnt = static_cast<int>( lua_tonumber( L, 2 ) );
    bool res = io->i2cIo( txCnt, rxCnt );
    lua_pushboolean( L, res ? 1 : 0 );
    return 1;
}

static int i2cStatus( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int status;
    bool res = io->i2cStatus( status );
    lua_pushboolean( L, res ? 1 : 0 );
    if ( res )
        lua_pushnumber( L, static_cast<lua_Number>( status ) );
    else
        lua_pushnumber( L, static_cast<lua_Number>( -1 ) );
    return 2;
}

static int i2cBuffer( lua_State * L )
{
    McuCtrl * io = *reinterpret_cast<McuCtrl * *>( lua_touserdata( L, 1 ) );
    int cnt = static_cast<int>( lua_tonumber( L, 2 ) );
    std::basic_string<unsigned char> vals;
    vals.resize( cnt );
    bool res = io->i2cBuffer( cnt, const_cast<unsigned char *>( vals.data() ) );
    if ( !res )
        return 0;
    for ( int i=0; i<cnt; i++ )
        lua_pushnumber( L, static_cast<lua_Number>( vals[i] ) );
    return cnt;
}


static const struct luaL_reg META_FUNCTIONS[] = {
    { "__gc",          gc },
    { "pointer",       self },
    // Open/close routines
    { "open",          open },
    { "close",         close },
    { "isOpen",        isOpen },
    // The lowest possible level
    { "write",         write },
    { "read",          read },
    { "setOutputs",    setOutputs },
    { "inputs",        inputs },
    { "accInit",       accInit },
    { "accAcc",        accAcc },
    { "accMag",        accMag },
    { "accTemp",       accTemp },

    { "i2cSetAddr", i2cSetAddr },
    { "i2cSetBuf",  i2cSetBuf },
    { "i2cIo",      i2cIo },
    { "i2cStatus",  i2cStatus },
    { "i2cBuffer",  i2cBuffer },

    { NULL,            NULL },
};

static void createMeta( lua_State * L )
{
    int top = lua_gettop( L );
    luaL_newmetatable( L, META );  // create new metatable for file handles
    // file methods
    lua_pushliteral( L, "__index" );
    lua_pushvalue( L, -2 );  // push metatable
    lua_rawset( L, -3 );       // metatable.__index = metatable
    luaL_register( L, NULL, META_FUNCTIONS );

    lua_settop( L, top );
}

static const struct luaL_reg FUNCTIONS[] = {
    { "create",  create },
    { NULL, NULL },
};

static void registerFunctions( lua_State * L )
{
    luaL_register( L, LIB_NAME, FUNCTIONS );
}

extern "C" int DECLSPEC luaopen_luamcuctrl( lua_State * L )
{
    createMeta( L );
    registerFunctions( L );
    return 0;
}





