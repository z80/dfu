
#include "luadeviceio.h"
#include "deviceio.h"

static const char META[] = "LUA_DEVICEIO_META";
static const char LIB_NAME[] = "luadeviceio";

static int create( lua_State * L )
{
    int cnt = lua_gettop( L );
    DeviceIo * io = new DeviceIo();
    DeviceIo * * p = reinterpret_cast< DeviceIo * * >( lua_newuserdata( L, sizeof( DeviceIo * ) ) );
    *p = dynamic_cast<DeviceIo *>( ab );
    luaL_getmetatable( L, META );
    lua_setmetatable( L, -2 );
    return 1;
}

static int gc( lua_State * L )
{
    DeviceIo * ab = *reinterpret_cast<DeviceIo * *>( lua_touserdata( L, 1 ) );
    if ( ab )
        ab->deleteLater();
    return 0;
}

static int self( lua_State * L )
{
    DeviceIo * ab = *reinterpret_cast<DeviceIo * *>( lua_touserdata( L, 1 ) );
    lua_pushlightuserdata( L, reinterpret_cast< void * >(ab) );
    return 1;
}

static int putUInt8( lua_State * L )
{
    DeviceIo * ab = *reinterpret_cast<DeviceIo * *>( lua_touserdata( L, 1 ) );
    unsigned char arg = static_cast<unsigned char>( lua_tonumber( L, 2 ) );
    int res = ab->putUInt8( arg );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int putUInt16( lua_State * L )
{
    DeviceIo * ab = *reinterpret_cast<DeviceIo * *>( lua_touserdata( L, 1 ) );
    unsigned short arg = static_cast<unsigned short>( lua_tonumber( L, 2 ) );
    int res = ab->putUInt16( arg );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int execFunc( lua_State * L )
{
    DeviceIo * ab = *reinterpret_cast<DeviceIo * *>( lua_touserdata( L, 1 ) );
    int arg = static_cast<int>( lua_tonumber( L, 2 ) );
    int res = ab->execFunc( arg );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
} 

static int readQueue( lua_State * L )
{
    DeviceIo * ab = *reinterpret_cast<DeviceIo * *>( lua_touserdata( L, 1 ) );
    int maxSize;
    if ( lua_gettop( L ) > 1 )
        maxSize = static_cast<int>( lau_tonumber( L, 2 ) );
    else
        maxSize = 128;
    std::basic_string<unsigned char> data;
    data.resize( maxSize );
    int cnt = ab.readQueue( const_cast<unsigned char *>( data.data() ), maxSize );
    lua_newtable( L );
    for ( int i=0; i<cnt; i++ )
    {
        lua_pushnumber( L, static_cast<lua_Number>( i+1 ) );
        lua_pushnumber( L, static_cast<lua_Number>( data[i] ) );
        lua_settable( L, -3 );
    }
    return 1;
}

static const struct luaL_reg META_FUNCTIONS[] = {
	{ "__gc",                    gc }, 
    { "pointer",                 self }, 
    // The lowest possible level
    { "pusUInt8",                putUInt8 }, 
    { "putUInt16",               putUInt16 }, 
    { "execFunc",                execFunc }, 
    { "readQueue",               readQueue }, 
    // Version
    { "version",                 version }, 
    // GPIO control
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

extern "C" int __declspec(dllexport) luaopen_luadeviceio( lua_State * L )
{
    createMeta( L );
    registerFunctions( L );
    return 0;
}





