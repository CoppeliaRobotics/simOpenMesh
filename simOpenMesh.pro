include(config.pri)

QT -= core
QT -= gui

TARGET = simOpenMesh
TEMPLATE = lib

DEFINES -= UNICODE
DEFINES += QT_COMPIL
DEFINES += SIM_MATH_DOUBLE
CONFIG += shared plugin
INCLUDEPATH += "../include"

*-msvc* {
    QMAKE_CXXFLAGS += -O2
    QMAKE_CXXFLAGS += -W3
}
*-g++* {
    QMAKE_CXXFLAGS += -O3
    QMAKE_CXXFLAGS += -Wall
    QMAKE_CXXFLAGS += -fvisibility=hidden
    QMAKE_CXXFLAGS += -Wno-unused-parameter
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-empty-body
    QMAKE_CXXFLAGS += -Wno-write-strings

    QMAKE_CXXFLAGS += -Wno-unused-but-set-variable
    QMAKE_CXXFLAGS += -Wno-unused-local-typedefs
    QMAKE_CXXFLAGS += -Wno-narrowing

    QMAKE_CFLAGS += -O3
    QMAKE_CFLAGS += -Wall
    QMAKE_CFLAGS += -Wno-strict-aliasing
    QMAKE_CFLAGS += -Wno-unused-parameter
    QMAKE_CFLAGS += -Wno-unused-but-set-variable
    QMAKE_CFLAGS += -Wno-unused-local-typedefs
}

SOURCES += \
    sourceCode/simOpenMesh.cpp \
    ../include/simLib/simLib.cpp \
    ../include/simStack/stackBool.cpp \
    ../include/simStack/stackNull.cpp \
    ../include/simStack/stackNumber.cpp \
    ../include/simStack/stackString.cpp \
    ../include/simStack/stackArray.cpp \
    ../include/simStack/stackMap.cpp \
    ../include/simStack/stackObject.cpp \
    ../include/simLib/scriptFunctionData.cpp \
    ../include/simLib/scriptFunctionDataItem.cpp \

HEADERS +=\
    sourceCode/simOpenMesh.h \
    ../include/simLib/simLib.h \
    ../include/simStack/stackBool.h \
    ../include/simStack/stackNull.h \
    ../include/simStack/stackNumber.h \
    ../include/simStack/stackString.h \
    ../include/simStack/stackArray.h \
    ../include/simStack/stackMap.h \
    ../include/simStack/stackObject.h \
    ../include/simLib/scriptFunctionData.h \
    ../include/simLib/scriptFunctionDataItem.h \

INCLUDEPATH += $$OPENMESH_INCLUDEPATH
LIBS += $$OPENMESH_LIBS

win32 {
    DEFINES += WIN_SIM
}

macx {
    DEFINES += MAC_SIM
}

unix:!macx {
    DEFINES += LIN_SIM
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

