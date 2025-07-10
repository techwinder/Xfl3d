QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TEMPLATE = app
TARGET = xfl3d

VERSION = 0.1

QT += opengl widgets network xml

greaterThan(QT_VERSION, 6)  {
   QT += openglwidgets
}

OBJECTS_DIR = ./objects
MOC_DIR     = ./moc
RCC_DIR     = ./rcc
DESTDIR     = .

CONFIG(release, debug|release) {
    CONFIG += optimize_full
}



linux-g++ {
    CONFIG += thread

    # VARIABLES
    isEmpty(PREFIX):PREFIX = /usr/local
    BINDIR = $$PREFIX/bin
    DATADIR = $$PREFIX/share/xfl3d

    desktop.path = $$DATADIR
    desktop.files += meta/linux/$${TARGET}.desktop

    icon128.path = $$DATADIR
    icon128.files += resources/icons/$${TARGET}.png

    target.path = $$BINDIR

    # MAKE INSTALL
    INSTALLS += target desktop icon128

}



win32-msvc {

    CONFIG -= debug_and_release debug_and_release_target
    CONFIG += no_batch

    RC_ICONS = meta/win64/Xfl3d.ico


#---------------- OTHER WIN LIBS -------------------
    DEFINES += _UNICODE WIN64 QT_DLL QT_WIDGETS_LIB
    LIBS += -lopengl32

    #hide the console
    LIBS += -lKernel32 -lUser32
}


RESOURCES += \
    resources/icons.qrc \
    resources/images.qrc


SOURCES += \
    globals/aboutxfl3d.cpp \
    globals/main.cpp \
    globals/mainframe.cpp \
    globals/prefsdlg.cpp \

HEADERS += \
    globals/aboutxfl3d.h \
    globals/mainframe.h \
    globals/prefsdlg.h



include(xfl3d/xfl3d.pri)
include(xflcore/xflcore.pri)
include(xflgeom/xflgeom.pri)
include(xflgraph/xflgraph.pri)
include(xflmath/xflmath.pri)
include(xflwidgets/xflwidgets.pri)


