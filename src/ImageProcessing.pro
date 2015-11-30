#-------------------------------------------------
#
# Project created by QtCreator 2015-10-14T20:46:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImageProcessing
TEMPLATE = app

INCLUDEPATH += C:\OpenCV\opencv\build_with_qt\install\include


LIBS += -LC:\OpenCV\opencv\build_with_qt\install\x86\vc11\lib -lopencv_calib3d2411 -lopencv_contrib2411 -lopencv_core2411 -lopencv_features2d2411 -lopencv_flann2411 -lopencv_gpu2411 -lopencv_highgui2411 -lopencv_imgproc2411 -lopencv_legacy2411 -lopencv_ml2411 -lopencv_nonfree2411 -lopencv_objdetect2411 -lopencv_ocl2411 -lopencv_photo2411 -lopencv_stitching2411 -lopencv_superres2411 -lopencv_ts2411 -lopencv_video2411 -lopencv_videostab2411

#LIBS += -LC:\OpenCV\opencv\build_with_qt\install\x86\vc11\lib -lopencv_calib3d2411d -lopencv_contrib2411d -lopencv_core2411d -lopencv_features2d2411d -lopencv_flann2411d -lopencv_gpu2411d -lopencv_highgui2411d -lopencv_imgproc2411d -lopencv_legacy2411d -lopencv_ml2411d -lopencv_nonfree2411d -lopencv_objdetect2411d -lopencv_ocl2411d -lopencv_photo2411d -lopencv_stitching2411d -lopencv_superres2411d -lopencv_ts2411d -lopencv_video2411d -lopencv_videostab2411d





SOURCES += main.cpp\
        mainwindow.cpp \
    medianfilter.cpp \
    gaussianfilterdialog.cpp \
    gaussianfilter.cpp \
    saturationdialog.cpp \
    saturationtool.cpp \
    sharpentool.cpp \
    liquifytool.cpp \
    centerimageview.cpp \
    buffingtool.cpp \
    whitentools.cpp \
    recolortool.cpp \
    recolordialog.cpp \
    recolorwidget.cpp \
    inverse_matrix.cpp

HEADERS  += mainwindow.h \
    medianfilter.h \
    gaussianfilterdialog.h \
    gaussianfilter.h \
    saturationdialog.h \
    saturationtool.h \
    sharpentool.h \
    liquifytool.h \
    centerimageview.h \
    buffingtool.h \
    whitentools.h \
    recolortool.h \
    recolordialog.h \
    recolorwidget.h \
    inverse_matrix.h

FORMS    += mainwindow.ui \
    gaussianfilterdialog.ui \
    saturationdialog.ui \
    recolordialog.ui
