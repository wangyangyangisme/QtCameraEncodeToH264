#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QImage>
#include "openglwidget.h"
#include "cameravideosurface.h"

class QCameraInfo;
class QCamera;
class QCameraImageCapture;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
signals:
public slots:
    void openCameraSlot();
    void stopCameraSlot();
    void captureSlot();
    void captureSlot(int  index , QImage image);
private:
    void createUi();
    void setUiStyleSheet();
private:
    QPushButton *m_OpenButton;
    QPushButton *m_StopButton;
    QPushButton *m_CaptureButton;
    QPushButton *m_QuitButton;
    QPushButton *m_SettingsButton;
    QComboBox *m_DeviceComboBox;
    QLabel *m_DeviceLabel;
    OPenGLWidget *m_VideoWidget;

    QList<QCameraInfo > m_CameraInfo;
    QCamera *m_Camera;
    CameraVideoSurface *m_CameraViewfinder;
    QCameraImageCapture *m_CameraImageCapture;
    int m_CurrentDevice;
};

#endif // MAINWINDOW_H
