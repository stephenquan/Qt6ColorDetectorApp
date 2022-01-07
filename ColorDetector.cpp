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

    int cB = 0;
    int cG = 0;
    int cR = 0;

    uchar* bits = image.bits();
    for (int y = 0; y < image.height(); y++)
    {
        for (int x = 0; x < image.width(); x++)
        {
            uchar B = bits[0];
            uchar G = bits[1];
            uchar R = bits[2];

            if (B > 128 && B > G && B > R)
                cB++;
            if (G > 128 && G > B && G > R)
                cG++;
            if (R > 128 && R > B && R > G)
                cR++;

            bits += 4;
        }
    }

    if (cB > cG && cB > cR)
    {
        emit colorDetected(QColor(Qt::GlobalColor::blue));
    }
    else if (cG > cB && cG > cR)
    {
        emit colorDetected(QColor(Qt::GlobalColor::green));
    }
    else if (cR > cB && cR > cG)
    {
        emit colorDetected(QColor(Qt::GlobalColor::red));
    }
}
