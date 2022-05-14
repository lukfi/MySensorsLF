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
#if 0
#if M_OS == M_OS_LINUX || M_OS == M_OS_WINDOWS
#include <cmath>
#include <cstdint>
#include <memory>
#include <cstring>

static char* dtostrf (double val, signed char width, unsigned char prec, char *sout)
{
    (void)width;
    uint32_t iPart = (uint32_t)val;
    sprintf(sout, "%d", iPart);

    if (prec > 0)
    {
        uint8_t pos = strlen(sout);
        sout[pos++] = '.';
        uint32_t dPart = (uint32_t)((val - (double)iPart) * pow(10, prec));

        for (uint8_t i = (prec - 1); i > 0; i--)
        {
            size_t pow10 = pow(10, i);
            if (dPart < pow10)
            {
                sout[pos++] = '0';
            }
            else
            {
                sout[pos++] = '0' + dPart / pow10;
                dPart = dPart % pow10;
            }
        }

        sout[pos++] = '0' + dPart;
        sout[pos] = '\0';
    }

    return sout;
}
#endif
#endif
#endif // DEFINES_H
