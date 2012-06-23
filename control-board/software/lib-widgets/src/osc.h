
#ifndef __OSC_H_
#define __OSC_H_

#include <QtGui>
#include "qwt_plot.h"

class QwtPlotCurve;

class Curve
{
public:
    Curve();
    ~Curve();
    Curve( const Curve & inst );
    const Curve & operator=( const Curve & inst );
    void resize( int sz );
    void attach( QwtPlot * plot );
    void prepare();

    QVector<qreal> x, y;
    int cnt;
    QwtPlotCurve * curve;
};

class Osc: public QwtPlot
{
    Q_OBJECT
public:
    Osc( QWidget * parent = 0 );
    ~Osc();
public slots:
    void setInterval( int ms );
    void start();
    void stop();

    void replot();

    void clear();
    void addData( int index, qreal val );
    void addDataXy( int index, qreal x, qreal y );

    void setCurvesCnt( int cnt );
    void setPointsCnt( int cnt );

    //void setLegendVisible( bool en );
signals:
    void timeout();
private:
    QVector<Curve> m_curves;
    QTimer * m_timer;
};





#endif


