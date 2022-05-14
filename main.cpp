#include "transport.h"
#include "mysensorslfgateway.h"
#include "utils/stringutils.h"

static Transport transport;

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
