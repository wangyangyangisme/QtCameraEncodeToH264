#ifndef CAPTUREWIDGET_H
#define CAPTUREWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPaintEvent>
#include <QPainter>

class CaptureWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CaptureWidget(const QImage &image, QWidget *parent = 0);
    ~CaptureWidget();
protected:
    void paintEvent(QPaintEvent *event);
signals:
public slots:
private:
    QImage m_Image;
};

#endif // CAPTUREWIDGET_H
