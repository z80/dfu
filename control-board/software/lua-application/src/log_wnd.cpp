
#include "log_wnd.h"
#include "ui_log_wnd.h"
#include <QtLua/State>

class LogWnd::PD
{
public:
    Ui_LogWnd ui;
    int linesCnt;
    QQueue<QString> l;
    QString log;
};

LogWnd::LogWnd( QWidget * parent, QtLua::State * st )
: QWidget( parent ), 
  Binder( st )
{
    pd = new PD();
    pd->linesCnt = 128;
    pd->ui.setupUi( this );

    connect( st,   SIGNAL(output(const QString &)), this, SLOT(log(const QString &)) );

    connect( this, SIGNAL(sigLog(const QString &)), this, SLOT(slotLog(const QString &)), Qt::QueuedConnection );
    connect( this, SIGNAL(sigLinesCnt(int)),        this, SLOT(slotLinesCnt(int)),        Qt::QueuedConnection );
    connect( this, SIGNAL(sigClear()),              this, SLOT(slotClear()),              Qt::QueuedConnection );

    connect( pd->ui.clear, SIGNAL(clicked()), this, SLOT(slotClear()), Qt::QueuedConnection );
}

LogWnd::~LogWnd()
{
    delete pd;
}

void LogWnd::handler()
{
    qApp->processEvents();
}

void LogWnd::echo( const std::string & stri )
{
    log( QString::fromStdString( stri ) );
}

bool LogWnd::resourceFile( const std::string & fileName, std::basic_string<char> & content )
{
    return false;
}

void LogWnd::log( const QString & stri )
{
    emit sigLog( stri );
}

void LogWnd::setLinesCnt( int cnt )
{
    emit sigLinesCnt( cnt );
}

void LogWnd::clear()
{
    emit sigClear();
}

void LogWnd::slotLog( const QString & stri )
{
    if ( stri.size() > 0 )
        pd->l.enqueue( stri );
    while ( pd->l.size() > pd->linesCnt )
        pd->l.dequeue();
    // Готовим к выводу.
    pd->log.clear();
    QQueue< QString >::iterator i;
    for ( i=pd->l.begin(); i!=pd->l.end(); i++ )
    {
        pd->log.append( *i );
        pd->log.append( "<br>" );
    }
    pd->ui.log->setHtml( pd->log );
    // Сдвигаем в конец текста.
    //moveCursor( QTextCursor::End, QTextCursor::MoveAnchor );
    // Слайдеры сдвинутся так, чтобы курсор был видимым.
    //ensureCursorVisible();
    pd->ui.log->verticalScrollBar()->setValue( pd->ui.log->verticalScrollBar()->maximum() );

}

void LogWnd::slotLinesCnt( int cnt )
{
    pd->linesCnt = cnt;
}

void LogWnd::slotClear()
{
    pd->l.clear();
    pd->ui.log->clear();
}


