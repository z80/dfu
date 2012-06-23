
#ifndef __C_OSC_SCALER_
#define __C_OSC_SCALER_

#include <QObject>
#include <QPen>
#include <QBrush>

class QwtPlot;
class QEvent;

class COscScaler: public QObject
{
    Q_OBJECT
public:
    COscScaler( QwtPlot * plot );
    virtual ~COscScaler();

    void setPen( const QPen & pen );
    const QPen & pen();
    void setBrush( const QBrush & brush );
    const QBrush & brush();
    void setEqualScales( bool en );
    bool equalScales();
    void setSaveScales( bool en );
    bool saveScales();
    void setWheelRatio( qreal ratio );
    qreal wheelRatio();
    void setWheelZoomX( bool en );
    void setWheelZoomY( bool en );
    
    virtual bool eventFilter( QObject *, QEvent * );
signals:
    void scaleXChanged( qreal, qreal );
    void scaleYChanged( qreal, qreal );
private:
    bool m_keyEqualScales;
    class PrivateData;
    PrivateData * m_pd;
    void paintX();
    void paintY();
    void dragXY( const QPoint & at );
    void scaleX( const QPoint & at );
    void scaleY( const QPoint & at );
    void mouseWheel( QEvent * e );
    void onResizeRescale( const QSize & size, const QSize & oldSize );
};


#endif


