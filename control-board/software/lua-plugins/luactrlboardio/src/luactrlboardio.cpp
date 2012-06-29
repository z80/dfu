
#include "luactrlboardio.h"
#include "ctrlboard_io.h"

static const char META[] = "LUA_CTRLBOARDIO_META";
static const char LIB_NAME[] = "luactrlboardio";

static int create( lua_State * L )
{
    int cnt = lua_gettop( L );
    CtrlboardIo * io = new CtrlboardIo();
    CtrlboardIo * * p = reinterpret_cast< CtrlboardIo * * >( lua_newuserdata( L, sizeof( CtrlboardIo * ) ) );
    *p = dynamic_cast<CtrlboardIo *>( io );
    luaL_getmetatable( L, META );
    lua_setmetatable( L, -2 );
    return 1;
}

static int gc( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    if ( io )
        delete io;
    return 0;
}

static int self( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    lua_pushlightuserdata( L, reinterpret_cast< void * >(io) );
    return 1;
}

static int open( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    bool res;
    if ( lua_gettop( L ) > 1 )
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
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    io->close();
    return 0;
}

static int isOpen( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    bool res = io->isOpen();
    lua_pushboolean( L, ( res ) ? 1 : 0 );
    return 1;
}

static int putUInt8( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    unsigned char arg = static_cast<unsigned char>( lua_tonumber( L, 2 ) );
    int res = io->putUInt8( arg );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int putUInt16( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    unsigned short arg = static_cast<unsigned short>( lua_tonumber( L, 2 ) );
    int res = io->putUInt16( arg );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
}

static int execFunc( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    int arg = static_cast<int>( lua_tonumber( L, 2 ) );
    int res = io->execFunc( arg );
    lua_pushnumber( L, static_cast<lua_Number>( res ) );
    return 1;
} 

static int readQueue( lua_State * L )
{
    CtrlboardIo * io = *reinterpret_cast<CtrlboardIo * *>( lua_touserdata( L, 1 ) );
    int maxSize;
    if ( lua_gettop( L ) > 1 )
        maxSize = static_cast<int>( lua_tonumber( L, 2 ) );
    else
        maxSize = 128;
    std::basic_string<unsigned char> data;
    data.resize( maxSize );
    int cnt = io->readQueue( const_cast<unsigned char *>( data.data() ), maxSize );
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
    // Open/close routines
    { "open",                    open }, 
    { "close",                   close }, 
    { "isOpen",                  isOpen }, 
    // The lowest possible level
    { "pusUInt8",                putUInt8 }, 
    { "putUInt16",               putUInt16 }, 
    { "execFunc",                execFunc }, 
    { "readQueue",               readQueue }, 

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

extern "C" int __declspec(dllexport) luaopen_luactrlboardio( lua_State * L )
{
    createMeta( L );
    registerFunctions( L );
    return 0;
}





