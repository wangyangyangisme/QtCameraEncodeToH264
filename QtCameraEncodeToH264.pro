QT += widgets gui multimedia multimediawidgets

CONFIG += c++11

win32{
    CONFIG += debug_and_release
    CONFIG(release , debug | release){
        target_path = ./build/release
    } else {
        target_path = ./build/debug
    }

    DESTDIR =./bin
    OBJECTS_DIR = $$target_path/obj
    RCC_DIR = $$target_path/rcc
    MOC_DIR = $$target_path/moc
    UI_DIR = $$target_path/ui
}

INCLUDEPATH += $$PWD/ffmpeg/include \

LIBS += $$PWD/ffmpeg/lib/avformat.lib \
        $$PWD/ffmpeg/lib/avcodec.lib \
        $$PWD/ffmpeg/lib/avdevice.lib \
        $$PWD/ffmpeg/lib/avfilter.lib \
        $$PWD/ffmpeg/lib/postproc.lib \
        $$PWD/ffmpeg/lib/swscale.lib \
        $$PWD/ffmpeg/lib/swresample.lib \
        $$PWD/ffmpeg/lib/avutil.lib
HEADERS += \
    mainwindow.h \
    cameravideosurface.h \
    openglwidget.h \
    capturewidget.h

SOURCES += \
    mainwindow.cpp \
    main.cpp \
    cameravideosurface.cpp \
    openglwidget.cpp \
    capturewidget.cpp

RESOURCES += \
    rc.qrc
