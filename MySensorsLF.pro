TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt


INCLUDEPATH +=  ../Common/System src

SOURCES += \
        src/MySensors/MyHelperFunctions.cpp \
        src/MySensors/MyMessage.cpp \
        main.cpp \
        src/mysensorslfgateway.cpp \
        transport.cpp

CONFIG(debug, debug|release) {
    LIBS += -L../CommonLibs/debug/
} else {
    LIBS += -L../CommonLibs/release/
}

LIBS += -lMultimedia -lSystem -latomic -lpthread

HEADERS += \
    src/MySensors/MyHelperFunctions.h \
    src/MySensors/MyIndication.h \
    src/MySensors/MyMessage.h \
    src/MySensors/Version.h \
    src/defines.h \
    src/mysensorslfgateway.h \
    transport.h
