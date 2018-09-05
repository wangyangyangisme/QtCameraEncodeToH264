#ifndef CAMERAVIDEOSURFACE_H
#define CAMERAVIDEOSURFACE_H

#include <QAbstractVideoSurface>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
}

class CameraVideoSurface : public QAbstractVideoSurface
{
    Q_OBJECT
public:
    CameraVideoSurface(QObject *parent = NULL);
    ~CameraVideoSurface();
    void setCameraResolution(const QSize &size);
protected:
    QList<QVideoFrame::PixelFormat> supportedPixelFormats(
            QAbstractVideoBuffer::HandleType handleType = QAbstractVideoBuffer::NoHandle) const override;
    bool present(const QVideoFrame &frame) override;
private slots:
    void cameraStopSlot();
signals:
    void showFrame(QImage image);
private:
    void InitEncoder();
    void Encode(AVFrame *frame);
private:
    AVFormatContext *pOutputFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVStream *pOutputStream;
    AVPacket *packet;
    AVFrame *yuvFrame;
    struct SwsContext *image_convert_ctx;
};

#endif // CAMERAVIDEOSURFACE_H
