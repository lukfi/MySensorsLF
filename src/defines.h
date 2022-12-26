#ifndef DEFINES_H
#define DEFINES_H

// defines from MySensorsCore.h
#define GATEWAY_ADDRESS         ((uint8_t)0)    //!< Node ID for GW sketch
#define NODE_SENSOR_ID          ((uint8_t)255)  //!< Node child is always created/presented when a node is started
#define MY_CORE_VERSION         ((uint8_t)2)    //!< core version
#define MY_CORE_MIN_VERSION     ((uint8_t)2)    //!< min core version required for compatibility

#define MY_WAKE_UP_BY_TIMER     ((int8_t)-1)    //!< Sleeping wake up by timer
#define MY_SLEEP_NOT_POSSIBLE   ((int8_t)-2)    //!< Sleeping not possible
#define INTERRUPT_NOT_DEFINED   ((uint8_t)255)  //!< _sleep() param: no interrupt defined
#define MODE_NOT_DEFINED        ((uint8_t)255)  //!< _sleep() param: no mode defined
#define VALUE_NOT_DEFINED       ((uint8_t)255)  //!< Value not defined
#define FUNCTION_NOT_SUPPORTED  ((uint16_t)0)   //!< Function not supported

#define MSG_GW_STARTUP_COMPLETE "Gateway startup complete." //!< Gateway startup message

#include <cmath>
#include <cstdint>
#include <memory>
#include <cstring>

extern char* dtostrf (double val, signed char width, unsigned char prec, char *sout);

#endif // DEFINES_H
