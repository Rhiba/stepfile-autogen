#-------------------------------------------------
#
# Project created by QtCreator 2016-02-29T16:50:21
#
#-------------------------------------------------

QT       += core gui
QMAKE_CXXFLAGS += -std=c++11\
                  -O0

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = stepfile-auto
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        processAudio.cpp\
        biquad.cpp\
        genStep.cpp\
        test.cpp\
        utils.cpp\
        wavelet2d.cpp\
        nn.cpp

HEADERS  += mainwindow.h\
            processAudio.h\
            wavelet2d.h\
            utils.h\
            test.h\
            genStep.h\
            datatypes.h\
            biquad.h\
            fftw3.h\
            nn.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../third/fftw/lib/release/ -lfftw3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../third/fftw/lib/debug/ -lfftw3
else:unix: LIBS += -L$$PWD/../third/fftw/lib/ -lfftw3

INCLUDEPATH += $$PWD/../third/fftw/include
DEPENDPATH += $$PWD/../third/fftw/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../third/fftw/lib/release/libfftw3.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../third/fftw/lib/debug/libfftw3.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../third/fftw/lib/release/fftw3.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../third/fftw/lib/debug/fftw3.lib
else:unix: PRE_TARGETDEPS += $$PWD/../third/fftw/lib/libfftw3.a

unix|win32: LIBS += -lsqlite3
