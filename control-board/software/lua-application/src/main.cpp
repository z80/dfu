
#include <QtGui>
#include <QUiLoader>
#include <QtLua/State>
#include "binder.h"
#include "script_editor.h"
#include "log_wnd.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::addLibraryPath( "./" );
    QtLua::State st;
    
    LogWnd * log = new LogWnd( 0, &st );
    ScriptEditor * se = new ScriptEditor( log, 0 );
    log->show();
    se->show();
    se->openFile( "./main.lua" );
 
    //QUiLoader loader;
    //qDebug() << loader.pluginPaths();
    //qDebug() << loader.availableWidgets();
    //loader.addPluginPath( "./" );
    //qDebug() << loader.pluginPaths();
    //qDebug() << loader.availableWidgets();
    //QFile file("./form.ui");
    //file.open( QFile::ReadOnly );
    //QWidget *myWidget = loader.load(&file, 0);
    //file.close();
    //myWidget->show();

    int res = app.exec();
    return res;
}

