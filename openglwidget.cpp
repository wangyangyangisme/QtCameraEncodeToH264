#include "openglwidget.h"
#include <QDebug>

OPenGLWidget::OPenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{

}

void OPenGLWidget::showCameraFrameSlot(QImage image)
{
    m_CameraFrame = image;
    this->update();
}

void OPenGLWidget::paintGL()
{
    QPainter painter(this);

    if (m_CameraFrame.size().width() <= 0)
    {
        return;
    }
    QImage _image = m_CameraFrame.scaled(this->size());
    painter.drawImage(0, 0, _image);
}
