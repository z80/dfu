
#ifndef __LOG_WND_H_
#define __LOG_WND_H_

#include <QtGui>
#include "binder.h"

class LogWnd: public QWidget, public Binder
{
    Q_OBJECT
public:
    LogWnd( QWidget * parent = 0, QtLua::State * st = 0 );
    ~LogWnd();

    void handler();
    void echo( const std::string & stri );
    bool resourceFile( const std::string & fileName, std::basic_string<char> & content );
public slots:
    void log( const QString & stri );
public:
    void setLinesCnt( int cnt );
    void clear();
signals:
    void sigLog( const QString & stri );
    void sigLinesCnt( int cnt );
    void sigClear();
private slots:
    void slotLog( const QString & log );
    void slotLinesCnt( int cnt );
    void slotClear();
private:
    class PD;
    PD * pd;
};

#endif



