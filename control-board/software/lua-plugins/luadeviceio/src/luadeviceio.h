
#ifndef __LUA_BAYSPEC_H_
#define __LUA_BAYSPEC_H_

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

extern "C" int __declspec(dllexport) luaopen_luabayspec( lua_State * L );


#endif

