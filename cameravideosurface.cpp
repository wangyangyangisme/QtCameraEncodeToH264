#include "cameravideosurface.h"
#include <QDebug>

#if 0
AVFormatContext *pOutputFormatCtx;
AVCodecContext *pCodecCtx;
AVCodec *pCodec;
AVStream *pOutputStream;
AVPacket *packet;
AVFrame *yuvFrame;
struct SwsContext *image_convert_ctx;
#endif

CameraVideoSurface::CameraVideoSurface(QObject *parent)
    : QAbstractVideoSurface(parent),
      pOutputFormatCtx(NULL),
      pCodec(NULL),
      pOutputStream(NULL),
      packet(NULL),
      yuvFrame(NULL),
      image_convert_ctx(NULL)
{
    this->InitEncoder();
}

CameraVideoSurface::~CameraVideoSurface()
{
    avformat_close_input(&pOutputFormatCtx);
    av_frame_free(&yuvFrame);
    av_packet_free(&packet);
    avcodec_close(pCodecCtx);
}

void CameraVideoSurface::setCameraResolution(const QSize &size)
{
    this->setNativeResolution(size);
}
QList<QVideoFrame::PixelFormat> CameraVideoSurface::supportedPixelFormats
(QAbstractVideoBuffer::HandleType handleType) const
{
    Q_UNUSED(handleType)
    QList<QVideoFrame::PixelFormat > pixelFormats;
    pixelFormats.append(QVideoFrame::Format_RGB32);
    pixelFormats.append(QVideoFrame::Format_YUV420P);
    return pixelFormats;
}

bool CameraVideoSurface::present(const QVideoFrame &frame)
{
    Q_UNUSED(frame)
    if (frame.isValid())
    {
        QVideoFrame cloneFrame(frame);
        cloneFrame.map(QAbstractVideoBuffer::ReadOnly);
        QImage image(cloneFrame.bits(), cloneFrame.width(), cloneFrame.height(),
                     QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat()));
        image = image.mirrored(true, true);

        // rgb to yuv
        uint8_t *data[AV_NUM_DATA_POINTERS] = {0};
        data[0] = (uint8_t *)image.constBits();
        int linesize[AV_NUM_DATA_POINTERS] = {0};
        linesize[0] = pCodecCtx->width * 4;
        sws_scale(image_convert_ctx, data, linesize, 0, pCodecCtx->height,
                  yuvFrame->data, yuvFrame->linesize);
        // Encode
        this->Encode(yuvFrame);
        emit showFrame(image);
        cloneFrame.unmap();
        return true;
    }
    return false;
}

void CameraVideoSurface::InitEncoder()
{
    av_register_all();
    avformat_network_init();
    avcodec_register_all();

    QString outputFileName = "output.h264";
    QString encoderName = "libx264";
    //QString rtmpAddress = "rtmp://192.168.1.111/live/livestream";

    pCodec = avcodec_find_encoder_by_name(encoderName.toStdString().c_str());
    if(NULL == pCodec)
    {
        qDebug() << __FUNCTION__ << "find video encoder failed !";
        return;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if(NULL == pCodecCtx)
    {
        qDebug() << __FUNCTION__ << "alloc codec context !";
        return;
    }
    // put sample parameter
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->width = 320;
    pCodecCtx->height = 240;
    pCodecCtx->time_base = {1, 25};
    pCodecCtx->framerate = {25, 1};
    pCodecCtx->gop_size = 10;
    pCodecCtx->max_b_frames = 1;
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if(AV_CODEC_ID_H264 == pCodecCtx->codec_id)
    {
        av_opt_set(pCodecCtx->priv_data, "preset", "slow", 0);
        av_opt_set(pCodecCtx->priv_data, "tune", "zerolatency", 0);
    }
    // open encoder
    if(avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        qDebug() << __FUNCTION__ << "open encoder failed !";
        return;
    }

#if 1
    pOutputFormatCtx = avformat_alloc_context();
    if(NULL == pOutputFormatCtx)
    {
        qDebug() << __FUNCTION__ << "avformat alloc failed !";
        return;
    }
    AVOutputFormat *outputFormat = av_guess_format(NULL, outputFileName.toStdString().c_str(), NULL);
    if(NULL == outputFormat)
    {
        qDebug() << __FUNCTION__ << "guess outputformat failed !";
        return;
    }
    pOutputFormatCtx->oformat = outputFormat;
#else
    int ret  = avformat_alloc_output_context2(&pOutputFormatCtx, NULL , "flv", rtmpAddress.toStdString().c_str());
    if(ret < 0)
    {
        qDebug() << __FUNCTION__ << "avformat_alloc_output_context2 failed !";
        return;
    }
    AVOutputFormat *outputFormat = av_guess_format(NULL, outputFileName.toStdString().c_str(), NULL);
    if(NULL == outputFormat)
    {
        qDebug() << __FUNCTION__ << "guess outputformat failed !";
        return;
    }
    pOutputFormatCtx->oformat = outputFormat;
#endif

#if 1
    // oprn url
    if(avio_open(&pOutputFormatCtx->pb, outputFileName.toStdString().c_str(), AVIO_FLAG_READ_WRITE) < 0)
    {
        qDebug() << __FUNCTION__ << "open output url failed !";
        return;
    }
#else
    // oprn url
    if(avio_open(&pOutputFormatCtx->pb, rtmpAddress.toStdString().c_str(), AVIO_FLAG_WRITE) < 0)
    {
        qDebug() << __FUNCTION__ << "open output url failed !";
        return;
    }
#endif
    pOutputStream = avformat_new_stream(pOutputFormatCtx, NULL);
    if(NULL == pOutputStream)
    {
        qDebug() << __FUNCTION__ << "avformat_new_stream failed !";
        return;
    }
    // dump informaton into standard IO
    av_dump_format(pOutputFormatCtx, 0, outputFileName.toStdString().c_str(), 1);
    // new packet
    packet = av_packet_alloc();
    if(NULL == packet)
    {
        qDebug() << __FUNCTION__ << "alloce AVPacket failed !";
        return;
    }
    // yuvFrame init
    yuvFrame = av_frame_alloc();
    if(NULL == yuvFrame)
    {
        qDebug() << __FUNCTION__ << "alloce AVFrame failed !";
        return;
    }
    yuvFrame->width = pCodecCtx->width;
    yuvFrame->height = pCodecCtx->height;
    yuvFrame->format = pCodecCtx->pix_fmt;
    // alloc image
    av_image_alloc(yuvFrame->data, yuvFrame->linesize, yuvFrame->width, yuvFrame->height,
                   pCodecCtx->pix_fmt, 32);
    // convert context
    image_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_RGB32, pCodecCtx->width,
                                       pCodecCtx->height, AV_PIX_FMT_YUV420P,
                                       SWS_BICUBIC, NULL, NULL, NULL);
    if(NULL == image_convert_ctx)
    {
        qDebug() << __FUNCTION__ << "getContext failed !";
        return;
    }
    // write header
    if(avformat_write_header(pOutputFormatCtx, NULL) < 0)
    {
        qDebug() << __FUNCTION__ << "avformat write header failed !";
        return;
    }
}

// 编码为 h.264
void CameraVideoSurface::Encode(AVFrame *frame)
{
    static int index = 0;
    frame->pts = index++;

    int ret = 0;
    if((ret = avcodec_send_frame(pCodecCtx, frame)) < 0)
    {
        qDebug() << __FUNCTION__ << "avcodec_send_frame fialed !";
        return;
    }

    while(ret >= 0)
    {
        ret = avcodec_receive_packet(pCodecCtx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return;
        }
        else if (ret < 0)
        {
            qDebug() << __FUNCTION__ << "Error during encoding";
            return;
        }
        packet->stream_index = 0;
        av_interleaved_write_frame(pOutputFormatCtx, packet); // write frame
        av_packet_unref(packet);
    }
}

void CameraVideoSurface::cameraStopSlot()
{
    av_write_trailer(pOutputFormatCtx);
}


