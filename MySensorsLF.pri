INCLUDEPATH += $$PWD/src $$PWD

SOURCES += \
        $$PWD/src/MySensors/MyHelperFunctions.cpp \
        $$PWD/src/MySensors/MyMessage.cpp \
        $$PWD/src/mysensorslfgateway.cpp \
        $$PWD/transport.cpp

HEADERS += \
    $$PWD/src/MySensors/MyHelperFunctions.h \
    $$PWD/src/MySensors/MyIndication.h \
    $$PWD/src/MySensors/MyMessage.h \
    $$PWD/src/MySensors/Version.h \
    $$PWD/src/defines.h \
    $$PWD/src/mysensorslfgateway.h \
    $$PWD/transport.h
