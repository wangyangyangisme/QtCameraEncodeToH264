#include "capturewidget.h"
#include <QDebug>

CaptureWidget::CaptureWidget(const QImage &image, QWidget *parent)
             : QWidget(parent), m_Image(image)
{
    this->setWindowTitle("Camera Capture");
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setAttribute(Qt::WA_ShowModal);
    this->resize(520, 360);
}

CaptureWidget::~CaptureWidget()
{

}

void CaptureWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    QImage image = m_Image.scaled(this->size());
    image = image.mirrored(true, false);
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}
