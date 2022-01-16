#include "ColorDetector.h"
#include <QMediaCaptureSession>
#include <QDebug>
#include <QSize>
#include <QDateTime>

ColorDetector::ColorDetector(QObject* parent) :
    QObject(parent),
    m_VideoSink(nullptr),
    m_Interval(1000),
    m_Last(0)
{
}

void ColorDetector::start(QObject* captureSession)
{
    if (!captureSession)
    {
        return;
    }

    QMediaCaptureSession* mediaCaptureSession = qobject_cast<QMediaCaptureSession*>(captureSession);
    if (!mediaCaptureSession)
    {
        return;
    }

    QVideoSink* videoSink = mediaCaptureSession->videoSink();
    if (!videoSink)
    {
        return;
    }

    m_VideoSink = videoSink;
    connect(m_VideoSink, &QVideoSink::videoFrameChanged, this, &ColorDetector::onVideoFrameChanged);
}

void ColorDetector::stop()
{
    if (!m_VideoSink)
    {
        return;
    }

    disconnect(m_VideoSink, &QVideoSink::videoFrameChanged, this, &ColorDetector::onVideoFrameChanged);
    m_VideoSink = nullptr;
}

void ColorDetector::onVideoFrameChanged()
{
    QVideoSink* videoSink = qobject_cast<QVideoSink*>(sender());
    if (!videoSink)
    {
        return;
    }

    qint64 current = QDateTime::currentMSecsSinceEpoch();
    if (current < m_Last + m_Interval)
    {
        return;
    }

    analyze(videoSink->videoFrame());

    m_Last = QDateTime::currentMSecsSinceEpoch();
}

void ColorDetector::analyze(const QVideoFrame& videoFrame)
{
    QImage image = videoFrame.toImage();
    analyze(image.width(), image.height(), image.bits());
}

void ColorDetector::analyze(int width, int height, uchar* bits)
{
    QMap<Qt::GlobalColor, int> freqs;

    if (width < 256 || height < 256)
    {
        return;
    }

    int cy = height / 2;
    int cx = width / 2;
    QRect area(cx - 128, cy - 128, 256, 256);
    for (int y = area.top(); y < area.bottom(); y++)
    {
        uchar* src = bits + 4 * width * y + 4 * area.left();
        for (int x = area.left(); x < area.right(); x++)
        {
            uchar B = src[0];
            uchar G = src[1];
            uchar R = src[2];

            Qt::GlobalColor _color = detectColorGroup(QColor(R, G, B));
            freqs[_color]++;

            src += 4;
        }
    }

    Qt::GlobalColor color = Qt::GlobalColor::black;
    int freq = freqs[color];
    QList<Qt::GlobalColor> list;
    list.append(Qt::GlobalColor::white);
    list.append(Qt::GlobalColor::red);
    list.append(Qt::GlobalColor::yellow);
    list.append(Qt::GlobalColor::green);
    list.append(Qt::GlobalColor::cyan);
    list.append(Qt::GlobalColor::blue);
    list.append(Qt::GlobalColor::magenta);
    foreach (Qt::GlobalColor _color, list)
    {
        int _freq = freqs[_color];
        if (_freq > freq)
        {
            freq = _freq;
            color = _color;
        }
    }

    emit colorDetected(QColor(color));
}

Qt::GlobalColor ColorDetector::detectColorGroup(const QColor& color)
{
    int h, s, v, a;
    color.getHsv(&h, &s, &v, &a);
    if (abs(h - 60) <= 30)
        return Qt::GlobalColor::yellow;
    if (abs(h - 120) <= 30)
        return Qt::GlobalColor::green;
    if (abs(h - 180) <= 30)
        return Qt::GlobalColor::cyan;
    if (abs(h - 240) <= 30)
        return Qt::GlobalColor::blue;
    if (abs(h - 300) <= 30)
        return Qt::GlobalColor::magenta;
    if (s > 128)
        return Qt::GlobalColor::red;
    return (v > 128) ? Qt::GlobalColor::white : Qt::GlobalColor::black;
}
