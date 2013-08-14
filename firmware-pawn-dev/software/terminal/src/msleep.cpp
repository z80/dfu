
#include "msleep.h"
#include <QtGui>

MSleep::MSleep()
: QtLua::Function()
{
}

MSleep::~MSleep()
{
}

QtLua::Value::List MSleep::meta_call( QtLua::State & ls, const QtLua::Value::List & args )
{
    meta_call_check_args( args, 1, 0, QtLua::Value::TNumber );
    int ms = args.first().to_integer();
    QTime time;
    time.start();
    while ( time.elapsed() < ms )
        qApp->processEvents();
    return QtLua::Value::List();
}

QtLua::String MSleep::get_description() const
{
    return "Stops script execution for provided number of milliseconds.";
}

QtLua::String MSleep::get_help() const
{
    return "msleep( <ms> ), ms - number of milliseconds script execution should be stopped for.";
}



