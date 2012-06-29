
#ifndef __LUA_DEVICEIO_H_
#define __LUA_DEVICEIO_H_

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

extern "C" int __declspec(dllexport) luaopen_luactrlboardio( lua_State * L );


#endif

