CONFIG -= qt

TARGET = vl_15

TEMPLATE = lib
VERSION = 1.0.0.4

DEFINES += VL15_LIBRARY

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/private_vl_15.cpp \
    src/ts.cpp \
    src/vl15_logic.cpp \
    src/vl_15.cpp

HEADERS += \
    src/VL15_global.h \
    VL_15.h \
    src/private_vl_15.h \
    src/ts.h \
    src/vl15_datatypes/cab/section1/elements.h \
    src/vl15_logic.h



win32-msvc*
{
   QMAKE_CXXFLAGS +=   /Zp4 #/MP
}

!isEmpty(target.path): INSTALLS += target
