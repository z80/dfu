
#ifndef __BINDER_H_
#define __BINDER_H_

#include <string>
#include <list>
//#include "boost::any.hpp"

namespace QtLua
{
    class State;
}
struct lua_State;
struct lua_Debug;
static void lineHook( lua_State * L, lua_Debug * ar );
// Overwriteble functions in Lua.
static int  print( lua_State * L );
static int  loadfile( lua_State * L );

class Binder
{
public:
    /*class Object
    {
    };

    class ObjectFactory
    {
    public:
        virtual const std::string & typename() = 0;
        virtual Object * create() = 0;
    }*/


    Binder( QtLua::State * state );
    virtual ~Binder();
    // For continous script execution.
    virtual void handler() = 0;
    // For print() function implementation.
    virtual void echo( const std::string & stri ) = 0;
    // To access resource files. If file doesn't exist it should get it from resource.
    virtual bool resourceFile( const std::string & fileName, std::basic_string<char> & content ) = 0;
    
    lua_State    * state();
    QtLua::State * qtState();
    //void enterCritical();
    //void leaveCritical();
    
    bool execFile( const std::string & fileName );
    bool execString( const std::string & string );
    void setCopyResourceFiles( bool en );
    // For debugging.
    void setTrace( bool en );
    bool isTrace() const;
    bool isRun() const;
    bool stopExec();
    bool breakExec();
    bool contExec();
    bool stepOver();
    bool stepInto();
    bool stepOut();
    bool eval( const std::string & stri );
    
protected:
    class PD;
    PD * pd;
    friend static void lineHook( lua_State * L, lua_Debug * ar );
    // Overwriteble functions in Lua.
    friend static int  print( lua_State * L );
    friend static int  loadfile( lua_State * L );
};

#endif

