
#ifndef __LUA_MCUCTRL_H_
#define __LUA_MCUCTRL_H_

extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

#ifndef DECLSPEC
    #define DECLSPEC
#endif

extern "C" int DECLSPEC luaopen_luamcuctrl( lua_State * L );


#endif

