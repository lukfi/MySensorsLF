#ifndef MYSENSORSLFGATEWAY_H
#define MYSENSORSLFGATEWAY_H

#include "MySensors/MyMessage.h"
#include <string>

typedef bool(*SendMessageFunc)(char* buf, void* cookie);
typedef void(*RecaiveMessageFunc)(MyMessage& rcv);

class MySensorsLFGateway
{
public:
    MySensorsLFGateway(SendMessageFunc sendMessageFunc = nullptr, RecaiveMessageFunc receiveMessageFunc = nullptr);
    void ClientConnected(void* cookie = nullptr);

    void SetSketchInfo(std::string source, std::string version = "", bool requestEcho = false);
    bool SetEntity(const uint8_t childSensorId, const mysensors_sensor_t sensorType, std::string description, mysensors_data_t dataType);

    MyMessage* GetMyMessage(const uint8_t sensorId);
    bool SendMyMessage(const uint8_t sensorId, bool requestEcho = false);

    void Receive(char* buf, uint32_t size);
private:
    MyMessage& BuildGw(MyMessage& msg, int8_t type);
    MyMessage& Build(MyMessage &msg, const uint8_t destination, const uint8_t sensor,  const mysensors_command_t command, const uint8_t type, const bool requestEcho = false);

    bool SendRoute(MyMessage &message);

    bool GatewayTransportSend(MyMessage& msg, void* cookie);

    void PresentNode();
    bool Present(const uint8_t childSensorId, const mysensors_sensor_t sensorType, const char *description = "", const bool requestEcho = false);
    void Presentation();
    bool SendSketchInfo(const std::string& name, const std::string& version, const bool requestEcho);

    uint8_t GetNodeId(void);

    char* ProtocolMyMessage2Serial(const MyMessage &message);
    bool ProtocolSerial2MyMessage(MyMessage &message, char *inputString);

    static const int MY_GATEWAY_MAX_SEND_LENGTH = 128;
    static const int MAX_ENTITIES = 8;

    char mMessageBuffer[MY_GATEWAY_MAX_SEND_LENGTH];
    char _convBuffer[MAX_PAYLOAD_SIZE * 2 + 1];

    static const int  MY_GATEWAY_MAX_RECEIVE_LENGTH = 100;
    char mReceiveBuffer[MY_GATEWAY_MAX_RECEIVE_LENGTH];
    uint8_t mReceiveBufferOccupancy {0};

    MyMessage mMsgTemp;
    MyMessage mEthernetMsg;

    SendMessageFunc mSendMessageFunc {nullptr};
    void* mCookie {nullptr};
    RecaiveMessageFunc mReceiveFunc {nullptr};

    struct
    {
        std::string mSource;
        std::string mVersion;
        bool mRequestEcho {false};
    } mSketchInfo;

    struct
    {
        uint8_t mId {0xff};
        mysensors_sensor_t mType;
        char mName[32];
        MyMessage mMessage;
    } mEntities[MAX_ENTITIES];
};

#endif // MYSENSORSLFGATEWAY_H
