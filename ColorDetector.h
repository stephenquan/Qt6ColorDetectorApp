#ifndef ColorDetector_H
#define ColorDetector_H

#include <QObject>
#include <QVariant>
#include <QVideoSink>
#include <QVideoFrame>
#include <QColor>

class ColorDetector : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int interval MEMBER m_Interval NOTIFY intervalChanged)

public:
    ColorDetector(QObject* parent = nullptr);

    Q_INVOKABLE void start(QObject* captureSession);
    Q_INVOKABLE void stop();

signals:
    void intervalChanged();
    void colorDetected(const QColor &color);

protected slots:
    void onVideoFrameChanged();

protected:
    QVideoSink* m_VideoSink;
    int m_Interval;
    qint64 m_Last;

    void analyze(const QVideoFrame& videoFrame);
    void analyze(int width, int height, uchar* bits);
    Qt::GlobalColor detectColorGroup(const QColor& color);

};

#endif
