
extern "C"
{
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}
#include "binder.h"
#include <sstream>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/bind/arg.hpp>
#include <boost/bind/placeholders.hpp>

#include <QtCore>
#include "QtLua/State"

class Binder::PD
{
public:
    PD() {}
    ~PD() {}

    lua_State * L;
    bool trace, 
         copyFiles, 
         running, 
         paused, 
         makeStep;
    QtLua::State * qst;

    static const std::string BINDER;
    static const std::string BINDER_PD;
    static const std::string LOADFILE;
    static void echoDebug( Binder * b, lua_Debug * ar );
    void placeBinder( Binder * b, lua_State * L );
    static void printOverwrite( Binder * b, lua_State * L );
    static void loadfileOverwrite( Binder * b, lua_State * L );
    static void pushBinder( lua_State * L );
    static void pushPd( lua_State * L );
    static void pushLoadfile( lua_State * L );

    void getLuaState( lua_State * L );
};

const std::string Binder::PD::BINDER    = "binder";
const std::string Binder::PD::BINDER_PD = "binder_pd";
const std::string Binder::PD::LOADFILE  = "binder_loadfile";

void Binder::PD::echoDebug( Binder * b, lua_Debug * ar )
{
    b->echo( ar->name );
    b->echo( ar->namewhat );
    b->echo( ar->what );
    //b->echo( ar->source );
    b->echo( ar->short_src );
    std::ostringstream os;
    os << ar->currentline;
    b->echo( os.str() );
}

void Binder::PD::placeBinder( Binder * b, lua_State * L )
{
    int t = lua_gettop( L );
    
    lua_pushstring( L, PD::BINDER.data() );
    lua_pushlightuserdata( L, reinterpret_cast<void *>( b ) );
    lua_settable( L, LUA_REGISTRYINDEX );
    
    lua_pushstring( L, PD::BINDER_PD.data() );
    lua_pushlightuserdata( L, reinterpret_cast<void *>( this ) );
    lua_settable( L, LUA_REGISTRYINDEX );
    
    lua_settop( L, t );
}

void Binder::PD::printOverwrite( Binder * b, lua_State * L )
{
    int t = lua_gettop( L );
    lua_pushstring( L, "print" );
    lua_pushcfunction( L, print );
    lua_settable( L, LUA_GLOBALSINDEX );
    
    //int ii = lua_gettop( L );
    //lua_pushstring( L, "print" );
    //lua_gettable( L, LUA_GLOBALSINDEX );
    //lua_pushstring( L, "assdfg" );
    //ii = lua_gettop( L );
    //lua_pcall( L, 1, 0, 0 );
    //lua_setglobal( L, "print" );
    
    lua_settop( L, t );
}

void Binder::PD::loadfileOverwrite( Binder * b, lua_State * L )
{
    int t = lua_gettop( L );
    lua_pushstring( L, "loadfile" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    lua_pushstring( L, LOADFILE.data() );
    lua_pushvalue( L, -2 );
    lua_settable( L, LUA_REGISTRYINDEX );
    lua_pushstring( L, "loadfile" );
    lua_pushcfunction( L, loadfile );
    lua_settable( L, LUA_GLOBALSINDEX );
    lua_settop( L, t );
}

void Binder::PD::pushBinder( lua_State * L )
{
    lua_pushstring( L, BINDER.data() );
    lua_gettable( L, LUA_REGISTRYINDEX );
}

void Binder::PD::pushPd( lua_State * L )
{
    lua_pushstring( L, BINDER_PD.data() );
    lua_gettable( L, LUA_REGISTRYINDEX );
}

void Binder::PD::pushLoadfile( lua_State * L )
{
    lua_pushstring( L, LOADFILE.data() );
    lua_gettable( L, LUA_REGISTRYINDEX );
}

void Binder::PD::getLuaState( lua_State * L )
{
    this->L = L;
}

QMutex gm;
lua_State * gL = 0;
void getLuaState( lua_State * L )
{
    gL = L;
}

Binder::Binder( QtLua::State * state )
{
    pd = new PD();
    pd->trace     = false;
    pd->copyFiles = true;
    pd->running   = false;
    pd->paused    = false;
    pd->makeStep  = false;
    pd->qst = state;
    
    state->openlib( QtLua::AllLibs );
    QMutexLocker lock( &gm );
    state->lua_do( getLuaState );
    pd->L = gL;
    
    lua_sethook( pd->L, lineHook, LUA_HOOKLINE, 0 );
    pd->placeBinder( this, pd->L );
    PD::printOverwrite( this, pd->L );
    PD::loadfileOverwrite( this, pd->L );
}

Binder::~Binder()
{
    if ( !pd->qst )
        lua_close( pd->L );
    delete pd;
}

lua_State * Binder::state()
{
    return pd->L;
}

QtLua::State * Binder::qtState()
{
    return pd->qst;
}

bool Binder::execFile( const std::string & fileName )
{
    pd->makeStep = false;
    if ( pd->running )
        return false;
    // Запуск файла с копированием или без копирования.
    FILE * fp = fopen( fileName.data(), "r" );
    if ( !fp )
    {
        std::basic_string<char> content;
        bool exists = resourceFile( fileName, content );
        if ( !exists )
        {
            std::ostringstream os;
            os << "ERROR: no such file \'" << fileName << "\'";
            echo( os.str() );
        }
        if ( pd->copyFiles )
        {
            fp = fopen( fileName.data(), "w" );
            if ( fp )
            {
                fwrite( content.data(), sizeof(char), content.size(), fp );
                fclose( fp );
            }
            else
            {
                std::ostringstream os;
                os << "ERROR: failed to copy file \'" << fileName << "\'";
                echo( os.str() );
            }
            // Execution from file.
            //PD::pushLoadfile( pd->L );
            //lua_pushstring( pd->L, fileName.data() );
            //pd->qst->exec_chunk( out );
            int err = luaL_dofile( pd->L, fileName.data() );
            switch ( err )
            {
            case LUA_ERRRUN:
                echo( "Runtime error" );
                break;
            case LUA_ERRMEM:
                echo( "Memory allocation error" );
                break;
            case LUA_ERRERR:
                echo( "error while running the error handler function" );
                break;
            }
            bool res = (err == 0);
            if ( !res )
            {
                int top = lua_gettop( pd->L );
                for ( int i=0; i<=top; i++ )
                {
                    std::string stri = lua_tostring( pd->L, i );
                    echo( stri );
                }
                lua_settop( pd->L, 0 );
            }
            return res;
        }
        else
        {
            // Execution from string.
            bool res = execString( content );
            return res;
        }

    }
    else
        fclose( fp );
    return false;
}

bool Binder::execString( const std::string & stri )
{
    pd->makeStep = false;
    if ( pd->running )
        return false;
    //int t = lua_gettop( pd->L );
    //lua_pushstring( pd->L, "print" );
    //lua_gettable( pd->L, LUA_GLOBALSINDEX );
    //lua_pushstring( pd->L, "assdfg" );
    //int ii = lua_gettop( pd->L );
    //ii = lua_pcall( pd->L, 1, 0, 0 );
    //ii = lua_gettop( pd->L );
    //lua_settop( pd->L, t );

    //ii = lua_gettop( pd->L );
    int err = luaL_dostring( pd->L, stri.data() );
    switch ( err )
    {
    case LUA_ERRRUN:
        echo( "Runtime error" );
        break;
    case LUA_ERRMEM:
        echo( "Memory allocation error" );
        break;
    case LUA_ERRERR:
        echo( "error while running the error handler function" );
        break;
    }
    bool res = (err == 0);
    if ( !res )
    {
        int top = lua_gettop( pd->L );
        for ( int i=1; i<=top; i++ )
        {
            std::string stri = lua_tostring( pd->L, i );
            echo( stri );
        }
        lua_settop( pd->L, 0 );
    }
    return res;
}

void Binder::setCopyResourceFiles( bool en )
{
    pd->copyFiles = en;
}

void Binder::setTrace( bool en )
{
    pd->trace = en;
}

bool Binder::isTrace() const
{
    return pd->trace;
}

bool Binder::isRun() const
{
    return pd->running;
}

bool Binder::stopExec()
{
    if ( pd->running )
    {
        int n = lua_gettop( pd->L );
        lua_Debug ar;
        lua_getstack( pd->L, 0, &ar );
        lua_getinfo( pd->L, "lS", &ar );
        lua_settop( pd->L, n );
        
        std::ostringstream os;
        os << "Execution interrupted at " << ar.short_src << ", line number " << ar.currentline;
        lua_pushstring( pd->L, os.str().data() );
        pd->running = false;
        lua_error( pd->L );
        return true;
    }
    return false;
}

bool Binder::breakExec()
{
    pd->paused = true;
    return pd->running;
}

bool Binder::contExec()
{
    pd->paused = false;
    return true;
}

bool Binder::stepOver()
{
    return pd->running;
}

bool Binder::stepInto()
{
    if ( pd->running )
        pd->makeStep = true;
    return pd->running;
}

bool Binder::stepOut()
{
    return pd->running;
}

bool Binder::eval( const std::string & stri )
{
    return false;
}

static void lineHook( lua_State * L, lua_Debug * ar )
{
    int n = lua_gettop( L );
    Binder::PD::pushPd( L );
    Binder::PD * pd = reinterpret_cast<Binder::PD *>( const_cast<void *>( lua_topointer( L, -1 ) ) );
    Binder::PD::pushBinder( L );
    Binder * b = reinterpret_cast<Binder *>( const_cast<void *>( lua_topointer( L, -1 ) ) );
    if ( pd->trace )
    {
        lua_Debug ar;
        lua_getstack( pd->L, 0, &ar );
        Binder::PD::echoDebug( b, &ar );
    }
    pd->running = true;
    // In handler pd->running could be overwritten in order to break the execution.
    // But should exec at least once.
    do {
        pd->makeStep = false;
        b->handler();
        if ( pd->makeStep )
        {
            pd->makeStep = false;
            break;
        }
    } while ( pd->paused );
    pd->running = false;
    lua_settop( L, n );
}

static int print( lua_State * L )
{
    int n = lua_gettop( L );
    Binder::PD::pushBinder( L );
    Binder * b = reinterpret_cast<Binder *>( const_cast<void *>( lua_topointer( L, -1 ) ) );
    lua_pushstring( L, "tostring" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    for ( int i=1; i<=n; i++ )
    {
        lua_pushvalue( L, -1 );
        lua_pushvalue( L, i );
        int res = lua_pcall( L, 1, 1, 0 );
        std::string stri;
        stri = lua_tostring( L, -1 );
        lua_pop( L, 1 );
        b->echo( stri );
    }
    lua_settop( L, n );
    return 0;
}

static int loadfile( lua_State * L )
{
    int n = lua_gettop( L );
    if ( n < 1 )
    {
        lua_pushnil( L );
        lua_pushstring( L, "ERROR: file name expected" );
    }
    // If file exists or not.
    std::string fileName = lua_tostring( L, 1 );
    FILE * fp = fopen( fileName.data(), "r" );
    if ( fp )
    {
        fclose( fp );
        Binder::PD::pushLoadfile( L );
        lua_pushvalue( L, 1 );
        int res = lua_pcall( L, 1, 2, 0 );
        int nres = lua_gettop( L );
        nres -= n;
        return nres;
    }
    Binder::PD::pushBinder( L );
    Binder * b = reinterpret_cast<Binder *>( const_cast<void *>( lua_topointer( L, -1 ) ) );
    
    std::basic_string<char> content;
    bool res = b->resourceFile( fileName, content );
    if ( !res )
    {
        lua_pushnil( L );
        std::ostringstream os;
        os << "ERROR: file not found \'" << fileName << "\'";
        lua_pushstring( L, os.str().data() );
    }
    lua_pushstring( L, "loadstring" );
    lua_gettable( L, LUA_GLOBALSINDEX );
    lua_pushstring( L, content.data() );
    int err = lua_pcall( L, 1, 2, 0 );
    int nres = lua_gettop( L );
    nres -= n;
    return nres;
}


