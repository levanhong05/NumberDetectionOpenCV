#-------------------------------------------------
#
# Project created by QtCreator 2016-10-23T12:46:30
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = OpenCVNumberDetection
CONFIG   += console

TEMPLATE = app

SOURCES +=  main.cpp \
            numberdetection.cpp \
            imageprocessor.cpp \
            config.cpp \
            knearestocr.cpp \
    plausi.cpp \
    videocaptures.cpp

HEADERS += \
            numberdetection.h \
            imageprocessor.h \
            config.h \
            knearestocr.h \
    plausi.h \
    videocaptures.h

win32 {
    INCLUDEPATH += "C:\\opencv2410\opencv\\build\\include" \

    CONFIG(debug,debug|release) {
        LIBS += -L"C:\\opencv2410\\opencv\\build\\x64\\vc12\\bin"
        LIBS += -L"C:\\opencv2410\\opencv\\build\\x64\\vc12\\lib"

        LIBS += -lopencv_core2410d \
                -lopencv_highgui2410d \
                -lopencv_imgproc2410d \
                -lopencv_features2d2410d \
                -lopencv_calib3d2410d \
                -lopencv_ml2410d
    }

    CONFIG(release,debug|release) {
        DEFINES += QT_NO_WARNING_OUTPUT QT_NO_DEBUG_OUTPUT
        LIBS += -L"C:\\opencv2410\\opencv\\build\\x64\\vc12\\bin"
        LIBS += -L"C:\\opencv2410\\opencv\\build\\x64\\vc12\\lib"

        LIBS += -lopencv_core2410 \
                -lopencv_highgui2410 \
                -lopencv_imgproc2410 \
                -lopencv_features2d2410 \
                -lopencv_calib3d2410 \
                -lopencv_ml2410
    }
}

win32 {
    SOURCEPATH = $$PWD/training
    CONFIG(debug,debug|release) {
        DESTPATH = $$OUT_PWD/debug/training
        copydata.commands = "$(COPY_DIR) $$replace(SOURCEPATH,/,\\) $$replace(DESTPATH,/,\\)"
    }

    CONFIG(release,debug|release) {
        DESTPATH = $$OUT_PWD/release/training
        copydata.commands = "$(COPY_DIR) $$replace(SOURCEPATH,/,\\) $$replace(DESTPATH,/,\\)"
    }
}

first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)

QMAKE_EXTRA_TARGETS += first copydata
