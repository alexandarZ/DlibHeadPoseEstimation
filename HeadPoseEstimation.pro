#-------------------------------------------------
#
# Project created by QtCreator 2017-09-24T11:56:46
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HeadPoseEstimation
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
        opengl_view.cpp \
        processing/iprocessingitem.cpp \
    processing/processingresult.cpp \
    processing/webcam.cpp \
    processing/framerender.cpp \
    headposeestimator.cpp \
    processing/imageutils.cpp \
    processing/face_detector.cpp \
    processing/head_posedetector.cpp \
    processing/face_snapchatdog.cpp \
    processing/face_fancyman.cpp \
    processing/face_swap.cpp \
    processing/face_vertex.cpp

HEADERS += \
        mainwindow.h \
        opengl_view.h \
        processing/iprocessingitem.h \
    processing/processingresult.h \
    processing/webcam.h \
    processing/framerender.h \
    headposeestimator.h \
    processing/imageutils.h \
    processing/face_detector.h \
    processing/head_posedetector.h \
    processing/face_snapchatdog.h \
    processing/face_fancyman.h \
    processing/face_swap.h \
    processing/face_vertex.h

FORMS += \
        mainwindow.ui

RESOURCES += \
    icons.qrc \
    overlays.qrc

#OpenCV library
INCLUDEPATH += $$PWD/../../Biblioteke/opencv-3.2.0/opencv-3.2.0/build/mingw/install/include
LIBS += -L$$PWD/../../Biblioteke/opencv-3.2.0/opencv-3.2.0/build/mingw/install/x86/mingw/lib  -llibopencv_core320.dll -llibopencv_highgui320.dll -llibopencv_videoio320.dll -llibopencv_imgproc320.dll -llibopencv_calib3d320.dll -llibopencv_imgcodecs320.dll -llibopencv_photo320.dll

#Dlib library
INCLUDEPATH += $$PWD/../../Biblioteke/dlib-19.4/dlib-19.4
LIBS += -L$$PWD/../../Biblioteke/dlib-19.4/dlib-19.4/build/mingw/dlib -llibdlib

#OpenGL
LIBS+= -lopengl32
