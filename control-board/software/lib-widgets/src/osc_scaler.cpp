
#include "osc_scaler.h"
#include <qwt_plot.h>
#include <qwt_scale_widget.h>
#include <qwt_plot_canvas.h>
#include <QEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QPainter>

class COscScaler::PrivateData
{
public:
    PrivateData()
    {
    }
    ~PrivateData()
    {
    }

    QwtPlot * m_plot;
    QwtScaleWidget * xBottom, 
                   * yLeft;
    QwtPlotCanvas  * canvas;
    QPen m_pen;
    QBrush m_brush;
    bool m_equalScales, 
         m_saveScales;

    bool m_paintX, 
         m_paintY, 
         m_dragX, 
         m_dragY, 
         m_scale;
    QPoint m_at;

    qreal xMinBase, xMaxBase, 
          yMinBase, yMaxBase, 
          
          wheelRatio;

    bool wheelZoomX, 
         wheelZoomY;
};

COscScaler::COscScaler( QwtPlot * plot )
    : QObject( plot )
{
    m_pd = new PrivateData();

    m_pd->m_plot = plot;
    m_pd->m_pen = Qt::NoPen;
    m_pd->m_brush = QBrush( QColor( 170, 170, 210, 150 ) );
    m_pd->m_equalScales = true;
    m_pd->m_saveScales = true;
    m_pd->m_paintX = false;
    m_pd->m_paintY = false;
    m_pd->m_dragX = false;
    m_pd->m_dragY = false;
    m_pd->m_scale = false;
    m_pd->wheelRatio = 1.2;
    m_pd->wheelZoomX = true;
    m_pd->wheelZoomY = true;
    m_keyEqualScales = false;

    m_pd->xBottom = plot->axisWidget( QwtPlot::xBottom );
    m_pd->xBottom->installEventFilter( this );
    // Чтобы получать события о перемещении когда мышь не нажата.
    //m_pd->xBottom->setMouseTracking( true );
    m_pd->yLeft = plot->axisWidget( QwtPlot::yLeft );
    m_pd->yLeft->installEventFilter( this );
    // Чтобы получать события о перемещении когда мышь не нажата.
    //m_pd->yLeft->setMouseTracking( true );
    m_pd->canvas = plot->canvas();
    m_pd->canvas->installEventFilter( this );
}

COscScaler::~COscScaler()
{
    delete m_pd;
}


void COscScaler::setPen( const QPen & pen )
{
    m_pd->m_pen = pen;
}

const QPen & COscScaler::pen()
{
    return m_pd->m_pen;
}


void COscScaler::setBrush( const QBrush & brush )
{
    m_pd->m_brush = brush;
}


const QBrush & COscScaler::brush()
{
    return m_pd->m_brush;
}


void COscScaler::setEqualScales( bool en )
{
    m_pd->m_equalScales = en;
}

bool COscScaler::equalScales()
{
    return m_pd->m_equalScales;
}


void COscScaler::setSaveScales( bool en )
{
    m_pd->m_saveScales = en;
}


bool COscScaler::saveScales()
{
    return m_pd->m_saveScales;
}


void COscScaler::setWheelRatio( qreal ratio )
{
    m_pd->wheelRatio = ratio;
}


qreal COscScaler::wheelRatio()
{
    return m_pd->wheelRatio;
}

void COscScaler::setWheelZoomX( bool en )
{
    m_pd->wheelZoomX = en;
}

void COscScaler::setWheelZoomY( bool en )
{
    m_pd->wheelZoomY = en;
}

bool COscScaler::eventFilter( QObject * obj, QEvent * e )
{
    QMouseEvent  * me;
    QKeyEvent    * ke;
    //QResizeEvent * re;
    if ( obj == m_pd->xBottom )
    {
        switch ( e->type() )
        {
        case QEvent::MouseMove:
            me = reinterpret_cast< QMouseEvent * >( e );
            // Подсвечивать или нет.
            m_pd->m_paintX = true;
            // Если включена прокрутка, вызываем прокрутку.
            if ( m_pd->m_dragX ) 
            {
                // А если включено масштабирование, вызываем масштабирование.
                if ( m_pd->m_scale )
                    scaleX( me->pos() );
                else
                    dragXY( me->pos() );
            }
            break;

        case QEvent::MouseButtonPress:
            m_pd->m_dragX = true;
            me = reinterpret_cast< QMouseEvent * >( e );
            m_pd->m_at = me->pos();
            m_pd->xMinBase = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->lowerBound();
            m_pd->xMaxBase = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->upperBound();
            m_pd->yMinBase = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->lowerBound();
            m_pd->yMaxBase = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->upperBound();
            break;

        case QEvent::MouseButtonRelease:
            m_pd->m_dragX = false;
            break;

        case QEvent::MouseButtonDblClick:
            m_pd->m_plot->setAxisAutoScale( QwtPlot::xBottom );
            m_pd->m_plot->replot();
            break;

        case QEvent::Leave:
            m_pd->m_paintX = false;
            m_pd->xBottom->releaseKeyboard();
            m_pd->xBottom->update();
            m_pd->m_scale = false;
            m_keyEqualScales = false;
            break;

        case QEvent::Enter:
            m_pd->m_paintX = true;
            m_pd->xBottom->grabKeyboard();
            m_pd->xBottom->update();
            break;

        case QEvent::Paint:
            // Рисуем то, что нужно было,
            obj->event( e );
            // А потом своё.
            if ( m_pd->m_paintX )
                paintX();
            return true;
        break;
        }
    }
    else if ( obj == m_pd->yLeft )
    {
        switch ( e->type() )
        {
        case QEvent::MouseMove:
            me = reinterpret_cast< QMouseEvent * >( e );
            // Подсвечивать или нет.
            m_pd->m_paintY = true;
            // Если включена прокрутка, вызываем прокрутку.
            if ( m_pd->m_dragY )
            {
                // А если включено масштабирование, вызываем масштабирование.
                if ( m_pd->m_scale )
                    scaleY( me->pos() );
                else
                    dragXY( me->pos() );
            }                
            break;

        case QEvent::MouseButtonPress:
            m_pd->m_dragY = true;
            me = reinterpret_cast< QMouseEvent * >( e );
            m_pd->m_at = me->pos();
            m_pd->xMinBase = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->lowerBound();
            m_pd->xMaxBase = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->upperBound();
            m_pd->yMinBase = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->lowerBound();
            m_pd->yMaxBase = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->upperBound();
            break;

        case QEvent::MouseButtonRelease:
            m_pd->m_dragY = false;
            break;

        case QEvent::MouseButtonDblClick:
            m_pd->m_plot->setAxisAutoScale( QwtPlot::yLeft );
            m_pd->m_plot->replot();
            break;

        case QEvent::Leave:
            m_pd->m_paintY = false;
            m_pd->yLeft->releaseKeyboard();
            m_pd->yLeft->update();
            m_pd->m_scale = false;
            m_keyEqualScales = false;
            break;

        case QEvent::Enter:
            m_pd->m_paintY = true;
            m_pd->yLeft->grabKeyboard();
            m_pd->yLeft->update();
            break;

        case QEvent::Paint:
            // Рисуем то, что нужно было,
            obj->event( e );
            // А потом своё.
            if ( m_pd->m_paintY )
                paintY();
            return true;
        }
    }
    else if ( obj == m_pd->canvas )
    {
        switch ( e->type() )
        {
        case QEvent::Wheel:
            mouseWheel( e );
            return false;
        }
    }
    
    if ( e->type() == QEvent::KeyPress )
    {
        ke = reinterpret_cast< QKeyEvent * >( e );
        switch( ke->key() )
        {
        case Qt::Key_Shift:
            m_pd->m_scale = true;
            break;
        case Qt::Key_Alt:
            m_pd->m_scale = true;
            m_keyEqualScales = true;
            break;
        }
    }
    else if ( e->type() == QEvent::KeyRelease )
    {
        ke = reinterpret_cast< QKeyEvent * >( e );
        switch( ke->key() )
        {
        case Qt::Key_Shift:
            m_pd->m_scale = false;
            break;
        case Qt::Key_Alt:
            m_pd->m_scale = false;
            m_keyEqualScales = false;
            break;
        }
    }    
    return false;
}










void COscScaler::paintX()
{
    QWidget * w = m_pd->m_plot->axisWidget( QwtPlot::xBottom );
    QPainter p( w );
    p.setPen( m_pd->m_pen );
    p.setBrush( m_pd->m_brush );
    p.drawRect( w->rect() );
}

void COscScaler::paintY()
{
    QWidget * w = m_pd->m_plot->axisWidget( QwtPlot::yLeft );
    QPainter p( w );
    p.setPen( m_pd->m_pen );
    p.setBrush( m_pd->m_brush );
    p.drawRect( w->rect() );
}

void COscScaler::dragXY( const QPoint & at )
{
    int dx = at.x() - m_pd->m_at.x();
    int sz = m_pd->xBottom->size().width();
    qreal rel = (qreal)dx / (qreal)sz;
    rel *= ( m_pd->xMaxBase - m_pd->xMinBase );
    qreal xMin = m_pd->xMinBase - rel;
    qreal xMax = m_pd->xMaxBase - rel;
    m_pd->m_plot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
    emit scaleXChanged( xMin, xMax );

    int dy = at.y() - m_pd->m_at.y();
    sz = m_pd->yLeft->size().height();
    rel = (qreal)dy / (qreal)sz;
    rel *= ( m_pd->yMaxBase - m_pd->yMinBase );
    qreal yMin = m_pd->yMinBase + rel;
    qreal yMax = m_pd->yMaxBase + rel;
    m_pd->m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
    emit scaleYChanged( yMin, yMax );

    m_pd->m_plot->replot();
}

void COscScaler::scaleX( const QPoint & at )
{
    int dx = at.x() - m_pd->m_at.x();
    int sz = m_pd->xBottom->size().width();
    qreal rel = 1.0 - (qreal)dx / (qreal)sz;
    qreal xMin = ( ( m_pd->xMaxBase + m_pd->xMinBase ) - rel * ( m_pd->xMaxBase - m_pd->xMinBase ) ) * 0.5;
    qreal xMax = ( ( m_pd->xMaxBase + m_pd->xMinBase ) + rel * ( m_pd->xMaxBase - m_pd->xMinBase ) ) * 0.5;
    m_pd->m_plot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
    emit scaleXChanged( xMin, xMax );
    if ( m_pd->m_equalScales || m_keyEqualScales )
    {
        qreal ratio = (qreal)m_pd->m_plot->canvas()->height() / (qreal)m_pd->m_plot->canvas()->width();
        rel *= ratio;
        qreal yMin = ( ( m_pd->yMaxBase + m_pd->yMinBase ) - rel * ( m_pd->xMaxBase - m_pd->xMinBase ) ) * 0.5;
        qreal yMax = ( ( m_pd->yMaxBase + m_pd->yMinBase ) + rel * ( m_pd->xMaxBase - m_pd->xMinBase ) ) * 0.5;
        m_pd->m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
        emit scaleYChanged( yMin, yMax );
    }
    else
    {
        int dy = at.y() - m_pd->m_at.y();
        sz = m_pd->yLeft->size().height();
        qreal rel = 1.0 + (qreal)dy / (qreal)sz;
        qreal yMin = ( ( m_pd->yMaxBase + m_pd->yMinBase ) - rel * ( m_pd->yMaxBase - m_pd->yMinBase ) ) * 0.5;
        qreal yMax = ( ( m_pd->yMaxBase + m_pd->yMinBase ) + rel * ( m_pd->yMaxBase - m_pd->yMinBase ) ) * 0.5;
        m_pd->m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
        emit scaleYChanged( yMin, yMax );
    }
    m_pd->m_plot->replot();
}

void COscScaler::scaleY( const QPoint & at )
{
    int dy = at.y() - m_pd->m_at.y();
    int sz = m_pd->yLeft->size().height();
    qreal rel = 1.0 + (qreal)dy / (qreal)sz;
    qreal yMin = ( ( m_pd->yMaxBase + m_pd->yMinBase ) - rel * ( m_pd->yMaxBase - m_pd->yMinBase ) ) * 0.5;
    qreal yMax = ( ( m_pd->yMaxBase + m_pd->yMinBase ) + rel * ( m_pd->yMaxBase - m_pd->yMinBase ) ) * 0.5;
    m_pd->m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
    emit scaleYChanged( yMin, yMax );
    if ( m_pd->m_equalScales || m_keyEqualScales )
    {
        qreal ratio = (qreal)m_pd->m_plot->canvas()->width() / (qreal)m_pd->m_plot->canvas()->height();
        rel *= ratio;
        qreal xMin = ( ( m_pd->xMaxBase + m_pd->xMinBase ) - rel * ( m_pd->yMaxBase - m_pd->yMinBase ) ) * 0.5;
        qreal xMax = ( ( m_pd->xMaxBase + m_pd->xMinBase ) + rel * ( m_pd->yMaxBase - m_pd->yMinBase ) ) * 0.5;
        m_pd->m_plot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
        emit scaleXChanged( xMin, xMax );
    }
    else
    {
        int dx = at.x() - m_pd->m_at.x();
        sz = m_pd->xBottom->size().width();
        rel = 1.0 - (qreal)dx / (qreal)sz;
        qreal xMin = ( ( m_pd->xMaxBase + m_pd->xMinBase ) - rel * ( m_pd->xMaxBase - m_pd->xMinBase ) ) * 0.5;
        qreal xMax = ( ( m_pd->xMaxBase + m_pd->xMinBase ) + rel * ( m_pd->xMaxBase - m_pd->xMinBase ) ) * 0.5;
        m_pd->m_plot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
        emit scaleXChanged( xMin, xMax );
    }
    m_pd->m_plot->replot();
}

void COscScaler::mouseWheel( QEvent * e )
{
    QWheelEvent * we = reinterpret_cast< QWheelEvent * >( e );
    int numDegrees = we->delta() / 8;
    int numSteps = numDegrees / 15;
    qreal zoom = (qreal)numSteps;
    zoom = ( zoom < 0.0 ) ? m_pd->wheelRatio : (1.0 / m_pd->wheelRatio);
    const QPoint & pt = we->pos();
    QPointF at = QPointF( m_pd->m_plot->invTransform( QwtPlot::xBottom, pt.x() ), 
                          m_pd->m_plot->invTransform( QwtPlot::yLeft,   pt.y() ) );
    // Для оси Ox.
    qreal boundLow  = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->lowerBound() - at.x();
    qreal boundHigh = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->upperBound() - at.x();
    if ( m_pd->wheelZoomX )
    {
        boundLow  *= zoom;
        boundHigh *= zoom;
        qreal xMin = boundLow + at.x();
        qreal xMax = boundHigh + at.x();
        m_pd->m_plot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
        emit scaleXChanged( xMin, xMax );
    }
    // То же для оси Oy.
    if ( m_pd->wheelZoomY )
    {
        boundLow  = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->lowerBound() - at.y();
        boundHigh = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->upperBound() - at.y();
        boundLow  *= zoom;
        boundHigh *= zoom;
        qreal yMin = boundLow + at.y();
        qreal yMax = boundHigh + at.y();
        m_pd->m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
        emit scaleYChanged( yMin, yMax );
    }
    m_pd->m_plot->replot();
}

void COscScaler::onResizeRescale( const QSize & size, const QSize & oldSize )
{
    if ( m_pd->m_saveScales )
    {
        qreal xMinBase = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->lowerBound();
        qreal xMaxBase = m_pd->m_plot->axisScaleDiv( QwtPlot::xBottom )->upperBound();
        qreal rel;
        if ( xMinBase < xMaxBase )
        {
            if ( ( size.width() > 0 ) && ( oldSize.width() > 0 ) )
            {
                rel = (qreal)size.width() / (qreal)oldSize.width();
                qreal xMin = ( ( xMinBase + xMaxBase ) - rel * ( xMaxBase - xMinBase ) ) * 0.5;
                qreal xMax = ( ( xMinBase + xMaxBase ) + rel * ( xMaxBase - xMinBase ) ) * 0.5;
                if ( xMin != xMax )
                {
                    m_pd->m_plot->setAxisScale( QwtPlot::xBottom, xMin, xMax );
                    emit scaleXChanged( xMin, xMax );
                }
            }
        }

        qreal yMinBase = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->lowerBound();
        qreal yMaxBase = m_pd->m_plot->axisScaleDiv( QwtPlot::yLeft )->upperBound();
        if ( yMinBase < yMaxBase )
        {
            if ( ( size.height() > 0 ) && ( oldSize.height() > 0 ) )
            {
                rel = (qreal)size.height() / (qreal)oldSize.height();
                qreal yMin = ( ( yMinBase + yMaxBase ) - rel * ( yMaxBase - yMinBase ) ) * 0.5;
                qreal yMax = ( ( yMinBase + yMaxBase ) + rel * ( yMaxBase - yMinBase ) ) * 0.5;
                if ( yMin != yMax )
                {
                    m_pd->m_plot->setAxisScale( QwtPlot::yLeft, yMin, yMax );
                    emit scaleYChanged( yMin, yMax );
                }
            }
        }
        m_pd->m_plot->replot();
    }
}




