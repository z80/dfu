
#include <QtGui>
#include "terminal.h"

int main( int argc, char * argv[] )
{
    QApplication app( argc, argv );
    Terminal * w = new Terminal();
    w->show();
    int res = app.exec();
    return res;
}


