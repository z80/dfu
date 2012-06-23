
#include "osc.h"
#include "qwt_plot_canvas.h"
#include "qwt_plot_layout.h"
#include "qwt_plot_grid.h"
#include "qwt_plot_curve.h"
#include "osc_scaler.h"

Curve::Curve()
{
    curve = new QwtPlotCurve();
    cnt = 0;
}

Curve::~Curve()
{
    delete curve;
}

Curve::Curve( const Curve & inst )
{
    curve = new QwtPlotCurve();
    *this = inst;
}

const Curve & Curve::operator=( const Curve & inst )
{
    if ( this != &inst )
    {
        x   = inst.x;
        y   = inst.y;
        cnt = inst.cnt;
    }
    return *this;
}

void Curve::resize( int sz )
{
    x.resize( sz );
    y.resize( sz );
}

void Curve::attach( QwtPlot * plot )
{
    curve->attach( plot );
}

void Curve::prepare()
{
    curve->setRawSamples( x.data(), y.data(), cnt );
}


Osc::Osc( QWidget * parent )
: QwtPlot( parent )
{
    COscScaler * scaler = new COscScaler( this );
    scaler->setWheelZoomX( true );
    scaler->setWheelZoomY( true );
    scaler->setEqualScales( false );
    scaler->setSaveScales( false );

    QwtPlotGrid * g = new QwtPlotGrid();
    g->enableXMin( true );
    g->enableYMin( true );
    g->setPen( QPen( Qt::gray, 0.0, Qt::DotLine ) );
    g->attach( this );

    g = new QwtPlotGrid();
    g->enableX( true );
    g->enableY( true );
    g->setPen( QPen( Qt::gray, 0.0, Qt::SolidLine ) );
    g->attach( this );

    canvas()->setBorderRadius( 10 );
    plotLayout()->setAlignCanvasToScales( true );

    // Nice background coloration.
    QPalette pal = canvas()->palette();
    QLinearGradient gr( 0.0, 0.0, 1.0, 1.0 );
    gr.setCoordinateMode( QGradient::StretchToDeviceMode );
    gr.setColorAt( 0.0, QColor( 200, 200, 230 ) );
    gr.setColorAt( 1.0, QColor( 230, 230, 250 ) );
    pal.setBrush( QPalette::Window, QBrush( gr ) );
    canvas()->setPalette( pal );
    canvas()->setBorderRadius( 10 );
    plotLayout()->setAlignCanvasToScales( true );

    m_timer = new QTimer( this );
    connect( m_timer, SIGNAL(timeout()), this, SIGNAL(timeout()) );
}

Osc::~Osc()
{
    m_timer->deleteLater();
}

void Osc::setInterval( int ms )
{
    m_timer->setInterval( ms );
}

void Osc::start()
{
    m_timer->start();
}

void Osc::stop()
{
    m_timer->stop();
}

void Osc::replot()
{
    for ( QVector<Curve>::iterator i=m_curves.begin(); i!=m_curves.end(); i++ )
        (*i).prepare();
    QwtPlot::replot();
}

void Osc::clear()
{
    for ( QVector<Curve>::iterator i=m_curves.begin(); i!=m_curves.end(); i++ )
        (*i).cnt = 0;
    replot();
}

void Osc::addData( int index, qreal val )
{
    Curve & c = m_curves[ index ];
    c.x[ c.cnt ] = static_cast<qreal>( c.cnt );
    c.y[ c.cnt ] = val;
    c.cnt = (c.cnt + 1) % c.x.size();
}

void Osc::addDataXy( int index, qreal x, qreal y )
{
    Curve & c = m_curves[ index ];
    c.x[ c.cnt ] = x;
    c.y[ c.cnt ] = y;
    c.cnt = (c.cnt + 1) % c.x.size();
}

static quint32 rand( quint32 xn )
{
    quint32 res1 = 36969 * ( xn & 0x0000FFFF ) + ( xn >> 16 );
    quint32 res2 = 18000 * ( xn & 0x0000FFFF ) + ( xn >> 16 );
    quint32 res = res1 + res2;
    return res;
}

void Osc::setCurvesCnt( int cnt )
{
    const quint32 c0 = 200;
    quint32 c = c0;
    m_curves.resize( cnt );
    for ( QVector<Curve>::iterator i=m_curves.begin(); i!=m_curves.end(); i++ )
    {
        QPen pen;
        pen.setWidth( 3 );
        pen.setColor( QColor( c ) );
        c = rand( c );
        (*i).curve->setPen( pen );
        (*i).attach( this );
    }
}

void Osc::setPointsCnt( int cnt )
{
    clear();
    for ( QVector<Curve>::iterator i=m_curves.begin(); i!=m_curves.end(); i++ )
        (*i).resize( cnt );
}

//void Osc::setLegendVisible( bool en )
//{
//}


