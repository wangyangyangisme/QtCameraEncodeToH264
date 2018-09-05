#include "mainwindow.h"
#include <QCamera>
#include <QCameraInfo>
#include <QApplication>
#include <QCameraImageCapture>
#include "capturewidget.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QWidget(parent),
          m_Camera(NULL),
          m_CurrentDevice(-1)
{
    this->resize(680, 380);
    this->setWindowTitle("Qt Camera Capture Encode To H.264");
    this->createUi();
    this->setUiStyleSheet();

    // find device
    m_CameraInfo = QCameraInfo::availableCameras();
    if(m_CameraInfo.isEmpty())
        QMessageBox::information(this, "Error", "can not find device !");
    foreach (QCameraInfo info, m_CameraInfo)
    {
        m_DeviceComboBox->addItem(info.description());
    }
    m_CameraViewfinder = new CameraVideoSurface();
    connect(m_OpenButton, SIGNAL(clicked(bool)), this, SLOT(openCameraSlot()));
    connect(m_StopButton, SIGNAL(clicked(bool)), this, SLOT(stopCameraSlot()));
    connect(m_StopButton, SIGNAL(clicked(bool)), m_CameraViewfinder, SLOT(cameraStopSlot()));
    connect(m_CaptureButton, SIGNAL(clicked(bool)), this, SLOT(captureSlot()));
    connect(m_QuitButton, SIGNAL(clicked(bool)), qApp, SLOT(quit()));
    connect(m_CameraViewfinder, SIGNAL(showFrame(QImage)), m_VideoWidget, SLOT(showCameraFrameSlot(QImage)));
}

MainWindow::~MainWindow()
{

}
void MainWindow::createUi()
{
    m_SettingsButton = new QPushButton(tr("Settings (&S)"));
    m_OpenButton = new QPushButton(tr("OPen (&O)"));
    m_StopButton = new QPushButton(tr("Stop (&S)"));
    m_CaptureButton = new QPushButton(tr("Capture (&C)"));
    m_QuitButton = new QPushButton(tr("Quit (&Q)"));
    m_DeviceLabel = new QLabel(tr("Camera Device: "));
    m_DeviceComboBox = new QComboBox();

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->addWidget(m_DeviceLabel);
    horizontalLayout->addWidget(m_DeviceComboBox);
    horizontalLayout->addWidget(m_SettingsButton);
    horizontalLayout->addWidget(m_OpenButton);
    horizontalLayout->addWidget(m_StopButton);
    horizontalLayout->addWidget(m_CaptureButton);
    horizontalLayout->addWidget(m_QuitButton);

    m_VideoWidget = new OPenGLWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(4, 4, 4, 6);
    mainLayout->addWidget(m_VideoWidget);
    mainLayout->addLayout(horizontalLayout);
}

void MainWindow::setUiStyleSheet()
{
    QString styleSheet;
    styleSheet.append("QPushButton{ color : rgb(255, 255, 255); \
                        border: 0px; border-radius: 5px; background-color : #0080FF;\
                        min-height: 24px; min-width: 78px;}\
                        QPushButton:hover{ background-color: #005DFF;}\
                        QPushButton:pressed{ background-color: #002AFF;}\
                        QLabel{ color: #1E1E27; }");
    // QComboBox:editable 设置右侧颜色
    styleSheet.append("QComboBox{ border : 1px solid  \"gray\"; border-radius: 3px; min-width : 80;\
                        min-height: 20px; padding: 1px 18px 1px 3px;}\
                        QComboBox:editable{ background : \"white\";}\
                        QComboBox:on{ padding-top: 3px; padding-left: 4px;}\
                        QComboBox:down-arrow{ image:url(:/image/array_down.png);}\
                        QComboBox:down-arrow:on{ top: 1px; left: 1px;}\
                        QComboBox::drop-down {\
                        subcontrol-origin: padding;\
                        subcontrol-position: top right;\
                        width: 15px;\
                        border-left-width: 1px;\
                        border-left-color: darkgray;\
                        border-left-style: solid; \
                        border-top-right-radius: 3px;\
                        border-bottom-right-radius: 3px;\
                        }");
                      this->setStyleSheet(styleSheet);
}
void MainWindow::openCameraSlot()
{
    if(NULL != m_Camera && m_CurrentDevice != m_DeviceComboBox->currentIndex())
    {
        qDebug() << __FUNCTION__ << "new camera";
        // delete old
        m_Camera->stop();
        delete m_Camera;
        m_Camera = NULL;
        disconnect(m_CameraImageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(captureSlot(int, QImage)));
        delete m_CameraImageCapture;
        m_CameraImageCapture = NULL;
    }

    QCameraInfo cameraInfo = m_CameraInfo.at(m_DeviceComboBox->currentIndex());
    m_CurrentDevice = m_DeviceComboBox->currentIndex();
    m_Camera = new QCamera(cameraInfo);
    m_Camera->setViewfinder(m_CameraViewfinder);
    m_CameraImageCapture = new QCameraImageCapture(m_Camera);
    m_Camera->start();
    connect(m_CameraImageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(captureSlot(int, QImage)));
}

void MainWindow::stopCameraSlot()
{
    m_Camera->stop();
}

void MainWindow::captureSlot()
{
    if(NULL == m_Camera)
        return;
    m_CameraImageCapture->capture();
}


void MainWindow::captureSlot(int index, QImage image)
{
    Q_UNUSED(index)
    Q_UNUSED(image)

    CaptureWidget *picWidget = new CaptureWidget(image);
    picWidget->show();
}

