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
    src/epk.cpp \
    src/private_vl_15.cpp \
    src/radiostation.cpp \
    src/saut.cpp \
    src/ts.cpp \
    src/utils/utils.cpp \
    src/vl15.cpp \
    src/vl_15_main.cpp

HEADERS += \
    src/VL15_global.h \
    VL_15.h \
    src/epk.h \
    src/private_vl_15.h \
    src/radiostation.h \
    src/saut.h \
    src/saut_datatype.h \
    src/shared_code.h \
    src/shared_structs.h \
    src/ts.h \
    src/utils/utils.h \
    src/vl15_datatypes/cab/section1/elements.h \
    src/vl15.h



win32-msvc*
{
   QMAKE_CXXFLAGS +=   /Zp4 #/MP
}

!isEmpty(target.path): INSTALLS += target
