#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QImage>
#include <QPainter>

class OPenGLWidget : public QOpenGLWidget
{
    Q_OBJECT
public:
    OPenGLWidget(QWidget *parent = NULL);
private slots:
    void showCameraFrameSlot(QImage image);
protected:
    void paintGL() override;
private:
    QImage m_CameraFrame;
};

#endif // OPENGLWIDGET_H
