#include "transport.h"
#include "src/mysensorslfgateway.h"
#include "utils/stringutils.h"

static Transport transport;

#if M_OS == M_OS_LINUX || M_OS == M_OS_WINDOWS
#include <cmath>
char* dtostrf (double val, signed char width, unsigned char prec, char *sout)
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

bool SendMyMessage(char* msg, void* data)
{
    transport.Send(msg, strlen(msg), data);
    return true;
}

static const char* gCommands[] = {"PRESENTATION",
                                  "SET",
                                  "REQ",
                                  "INTERNAL",
                                  "STREAM",
                                  "RESERVED_5",
                                  "RESERVED_6",
                                  "INVALID_7"};

bool gLightOn = false;

std::string MyMessage2String(MyMessage& msg)
{
    std::string ret;

    LF::utils::sformat(ret, "%s %d", gCommands[msg.getCommand()], msg.getSensor());

    return ret;
}

static MySensorsLFGateway* sensors = nullptr;

void ReceiveMyMessage(MyMessage& msg)
{
    printf("%s\n", MyMessage2String(msg).c_str());
    gLightOn = msg.getBool();
    sensors->GetMyMessage(3)->set(gLightOn);
    sensors->SendMyMessage(3);
}

static MySensorsLFGateway mySensors(SendMyMessage, ReceiveMyMessage);

void OnConnected(void* cookie)
{
    mySensors.ClientConnected(cookie);
    mySensors.GetMyMessage(3)->set(gLightOn);
    mySensors.SendMyMessage(3);
}

void OnRead(char* buf, uint32_t len)
{
    mySensors.Receive(buf, len);
}

#define PRESENTATION_SOURCE "HASS A"
#define PRESENTATION_VERSION "6.0.1"
#define CORE_VERSION "6.0.1"

int main()
{
    sensors = &mySensors;

    mySensors.SetSketchInfo(PRESENTATION_SOURCE, PRESENTATION_VERSION, false);
    mySensors.SetEntity(1, S_DOOR, "drzwi1", V_TRIPPED);
    mySensors.SetEntity(2, S_DOOR, "drzwi2", V_TRIPPED);
    mySensors.SetEntity(3, S_LIGHT, "swiatlo", V_STATUS);
    CONNECT(transport.CONNECTED, OnConnected);
    CONNECT(transport.READ, OnRead);
    transport.Open(5555);
    transport.Join();
}
