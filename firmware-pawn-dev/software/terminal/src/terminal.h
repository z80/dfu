
#ifndef __TERMINAL_H_
#define __TERMIANL_H_

#include "ui_terminal.h"
#include "QtLua/State"
#include "msleep.h"

class MSleep;

class Terminal: public QMainWindow
{
    Q_OBJECT
public:
    Terminal( QWidget * parent = 0 );
    ~Terminal();
    
protected:
    void closeEvent( QCloseEvent * e );
private slots:
    void slotRun();
private:
    Ui_Terminal ui;
    QPointer<QtLua::State> state;
    static MSleep msleep;
};



#endif



