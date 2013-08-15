
#include "terminal.h"
#include "luamcuctrl.h"
#include <QtGui>

MSleep Terminal::msleep;

static void reg_register( lua_State * L )
{
    luaopen_luamcuctrl( L );
}


Terminal::Terminal( QWidget * parent )
: QMainWindow( parent )
{
    ui.setupUi( this );

    state = new QtLua::State();
    state->openlib( QtLua::AllLibs );
    msleep.register_( *state, "msleep" );
    // Register luaftdi module.
    state->lua_do( reg_register );

    QSettings settings( "history.ini", QSettings::IniFormat, this );
    ui.terminal->load_history( settings );

    QObject::connect( ui.terminal, SIGNAL(line_validate(const QString&)),
                      state, SLOT(exec(const QString&)) );

    QObject::connect( ui.terminal, SIGNAL(get_completion_list(const QString &, QStringList &, int &)),
                      state, SLOT(fill_completion_list(const QString &, QStringList &, int &)) );

    QObject::connect( state, SIGNAL(output(const QString&)),
                      ui.terminal, SLOT(print(const QString&)) );

    ui.terminal->print( "You may type: help(), list() and use TAB completion.\n" );
    ui.terminal->print( "bootHv( <slot index> ) - booting HV FPGA chip.\n" );
    ui.terminal->print( "enum( <slot index> )   - attempt to get enum from slot provided.\n" );

    connect( ui.run, SIGNAL(triggered()), this, SLOT(slotRun()) );

    QFile file( "default.lua" );
    if ( file.exists() )
    {
        if ( file.open( QIODevice::ReadOnly ) )
        {
            try
            {
                state->exec_chunk( file );
            } catch ( QtLua::String & e )
            {
                ui.terminal->print( e );
            }
        }
    }
}

Terminal::~Terminal()
{
}

void Terminal::closeEvent( QCloseEvent * e )
{
    state->deleteLater();
    QSettings settings( "history.ini", QSettings::IniFormat, this );
    ui.terminal->save_history( settings );
}

void Terminal::slotRun()
{
    QString stri = 
    QFileDialog::getOpenFileName( this, tr("Open Lua script"),
                                        "",
                                        tr("Lua (*.lua)"));
    QFile file( stri );
    if ( file.exists() )
    {
        if ( file.open( QIODevice::ReadOnly ) )
            state->exec_chunk( file );
    }
}


