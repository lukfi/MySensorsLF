TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH +=  ../Common/System MySensors

SOURCES += \
        MySensors/MyHelperFunctions.cpp \
        MySensors/MyMessage.cpp \
        main.cpp \
        mysensorslfgateway.cpp \
        transport.cpp

CONFIG(debug, debug|release) {
    LIBS += -L../CommonLibs/debug/
} else {
    LIBS += -L../CommonLibs/release/
}

LIBS += -lMultimedia -lSystem -latomic -lpthread

HEADERS += \
    MySensors/MyHelperFunctions.h \
    MySensors/MyIndication.h \
    MySensors/MyMessage.h \
    MySensors/Version.h \
    defines.h \
    mysensorslfgateway.h \
    transport.h
