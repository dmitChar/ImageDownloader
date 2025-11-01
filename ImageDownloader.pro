QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17
LIBS += -lopencv_core -lopencv_imgproc -lopencv_imgcodecs -lopencv_dnn_superres -lopencv_dnn


macx {
    # Для Homebrew на Apple Silicon
    INCLUDEPATH += /opt/homebrew/opt/opencv/include/opencv4
    LIBS += -L/opt/homebrew/opt/opencv/lib \
        -lopencv_core \
        -lopencv_imgproc \
        -lopencv_highgui \
        -lopencv_videoio \
        -lopencv_imgcodecs \
        -lopencv_video \
        -lopencv_objdetect
} else {
    # Для других систем
    CONFIG += link_pkgconfig
    PKGCONFIG += opencv4
}

SOURCES += \
    downloadmanager.cpp \
    main.cpp \
    mainwindow.cpp \
    scaleimage.cpp

HEADERS += \
    downloadmanager.h \
    mainwindow.h \
    scaleimage.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
