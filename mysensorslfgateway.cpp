#include "mysensorslfgateway.h"
#include <memory>

#include "defines.h"
#include "MyIndication.h"
#include "MyHelperFunctions.h"
#include "Version.h"

/********** DEBUG SETUP **********/
#define ENABLE_SDEBUG
#define DEBUG_PREFIX "MySensorsLFGateway: "
#include "utils/singleton.h"
#include "utils/screenlogger.h"
/*********************************/

MySensorsLFGateway::MySensorsLFGateway(SendMessageFunc sendMessageFunc, RecaiveMessageFunc receiveMessageFunc) :
    mSendMessageFunc(sendMessageFunc),
    mReceiveFunc(receiveMessageFunc)
{

}

void MySensorsLFGateway::ClientConnected(void* cookie)
{
    if (mSendMessageFunc)
    {
        mCookie = cookie;
        GatewayTransportSend(BuildGw(mMsgTemp, I_GATEWAY_READY).set(MSG_GW_STARTUP_COMPLETE), mCookie);
        PresentNode();
    }
}

void MySensorsLFGateway::SetSketchInfo(std::string source, std::string version, bool requestEcho)
{
    mSketchInfo.mSource = source;
    mSketchInfo.mVersion = version;
    mSketchInfo.mRequestEcho = requestEcho;
}

bool MySensorsLFGateway::SetEntity(const uint8_t childSensorId, const mysensors_sensor_t sensorType, std::string description, mysensors_data_t dataType)
{
    bool ret = false;

    if (childSensorId != 0xff)
    {
        int newIndex = -1;
        bool duplicateId = false;
        for (int i = 0; i < MAX_ENTITIES; ++i)
        {
            if (mEntities[i].mId == 0xff && newIndex == -1)
            {
                newIndex = i;
            }
            else if (mEntities[i].mId == childSensorId)
            {
                newIndex = -1;
                duplicateId = true;
                SWARN("Entity with id: %d already exists!", childSensorId);
                break;
            }
        }
        if (newIndex != -1)
        {
            mEntities[newIndex].mId = childSensorId;
            mEntities[newIndex].mType = sensorType;
            strcpy(mEntities[newIndex].mName, description.c_str());
            mEntities[newIndex].mMessage.clear();
            mEntities[newIndex].mMessage.setSensor(childSensorId);
            mEntities[newIndex].mMessage.setType(dataType);
            ret = true;
            SDEB("Added entity[%d]: %s, type:%d", mEntities[newIndex].mId, mEntities[newIndex].mName, mEntities[newIndex].mType);
        }
        else if (!duplicateId)
        {
            SWARN("Entity list full!");
        }
    }
    return ret;
}

MyMessage* MySensorsLFGateway::GetMyMessage(const uint8_t sensorId)
{
    MyMessage* ret = nullptr;
    for (int i = 0; i < MAX_ENTITIES; ++i)
    {
        if (mEntities[i].mId == sensorId)
        {
            ret = &mEntities[i].mMessage;
            break;
        }
    }
    return ret;
}

bool MySensorsLFGateway::SendMyMessage(const uint8_t sensorId, bool requestEcho)
{
    for (int i = 0; i < MAX_ENTITIES; ++i)
    {
        if (mEntities[i].mId == sensorId)
        {
            MyMessage& message = mEntities[i].mMessage;
            message.setSender(GetNodeId());
            message.setCommand(C_SET);
            message.setRequestEcho(requestEcho);

            return SendRoute(message);
        }
    }
    return false;
}

void MySensorsLFGateway::Receive(char *buf, uint32_t size)
{
    for (uint32_t pos = 0; pos < size; ++pos)
    {
        const char inChar = buf[pos];
        if (mReceiveBufferOccupancy < MY_GATEWAY_MAX_RECEIVE_LENGTH - 1)
        {
            // if newline then command is complete
            if (inChar == '\n' || inChar == '\r')
            {
                // Add string terminator and prepare for the next message
                //GATEWAY_DEBUG(PSTR("GWT:RFC:C=%" PRIu8 ",MSG=%s\n"), i, inputString[i].string);
                SINFO("<- %s\n", mReceiveBuffer);
                mReceiveBuffer[mReceiveBufferOccupancy] = 0;
                mReceiveBufferOccupancy = 0;
                if (ProtocolSerial2MyMessage(mEthernetMsg, mReceiveBuffer))
                {
                    if (mEthernetMsg.isAck() || mEthernetMsg.isAck())
                    {
                        SWARN("ECHO: %d ACK: %d", mEthernetMsg.isAck(), mEthernetMsg.isAck());
                    }
                    if (mEthernetMsg.getCommand() == C_INTERNAL)
                    {
                        SDEB("INTERNAL: %d", mEthernetMsg.getType());
                        if (mEthernetMsg.getType() == I_VERSION)
                        {
                            // Request for version. Create the response
                            GatewayTransportSend(BuildGw(mMsgTemp, I_VERSION).set(MYSENSORS_LIBRARY_VERSION), mCookie);
                        }
                        else
                        {
                            SERR("Unknown type: %d", mEthernetMsg.getType());
                        }
                    }
                    else if (mReceiveFunc)
                    {
                        mReceiveFunc(mEthernetMsg);
                    }
                }
            }
            else
            {
                // add it to the inputString:
                //inputString[i].string[inputString[i].idx++] = inChar;
                mReceiveBuffer[mReceiveBufferOccupancy++] = inChar;
            }
        }
        else
        {
            // Incoming message too long. Throw away
            //GATEWAY_DEBUG(PSTR("!GWT:RFC:C=%" PRIu8 ",MSG TOO LONG\n"), i);
            SERR("Message to long");
            mReceiveBufferOccupancy = 0;
            // Finished with this client's message. Next loop() we'll see if there's more to read.
            break;
        }
    }
}

MyMessage& MySensorsLFGateway::Build(MyMessage &msg, const uint8_t destination, const uint8_t sensor,  const mysensors_command_t command, const uint8_t type, const bool requestEcho)
{
    msg.setSender(GetNodeId());
    msg.setDestination(destination);
    msg.setSensor(sensor);
    msg.setType(type);
    msg.setCommand(command);
    msg.setRequestEcho(requestEcho);
    msg.setEcho(false);
    return msg;
}

MyMessage& MySensorsLFGateway::BuildGw(MyMessage& msg, int8_t type)
{
    msg.setSender(GATEWAY_ADDRESS);
    msg.setDestination(GATEWAY_ADDRESS);
    msg.setSensor(NODE_SENSOR_ID);
    msg.setType(type);
    msg.setCommand(C_INTERNAL);
    msg.setRequestEcho(false);
    msg.setEcho(false);
    return msg;
}

bool MySensorsLFGateway::SendRoute(MyMessage &message)
{
//#if defined(MY_CORE_ONLY)
//    (void)message;
//#endif
//#if defined(MY_GATEWAY_FEATURE)
    if (message.getDestination() == GetNodeId())
    {
        // This is a message sent from a sensor attached on the gateway node.
        // Pass it directly to the gateway transport layer.
        return GatewayTransportSend(message, mCookie);
    }
//#endif
//#if defined(MY_SENSOR_NETWORK)
//    return transportSendRoute(message);
//#else
//    return false;
//#endif
    return false;
}

bool MySensorsLFGateway::GatewayTransportSend(MyMessage&msg, void* cookie)
{
    bool ret = false;
    if (mSendMessageFunc && cookie)
    {
        char* buf = ProtocolMyMessage2Serial(msg);
        SINFO("-> %s", buf);
        ret = mSendMessageFunc(buf, cookie);
    }
    return ret;
}

void MySensorsLFGateway::PresentNode()
{
    //setIndication(INDICATION_PRESENT);
    Present(NODE_SENSOR_ID, S_ARDUINO_NODE);
    Presentation();
}

bool MySensorsLFGateway::Present(const uint8_t childSensorId, const mysensors_sensor_t sensorType, const char *description, const bool requestEcho)
{
    return SendRoute(Build(mMsgTemp, GATEWAY_ADDRESS, childSensorId, C_PRESENTATION,
                           static_cast<uint8_t>(sensorType),
                           requestEcho).set(childSensorId == NODE_SENSOR_ID ? MYSENSORS_LIBRARY_VERSION : description));
}

void MySensorsLFGateway::Presentation()
{
    SendSketchInfo(mSketchInfo.mSource, mSketchInfo.mVersion, mSketchInfo.mRequestEcho);
    for (int i = 0; i < MAX_ENTITIES; ++i)
    {
        if (mEntities[i].mId != 0xff)
        {
            Present(mEntities[i].mId, mEntities[i].mType, mEntities[i].mName);
        }
    }
}

bool MySensorsLFGateway::SendSketchInfo(const std::string &name, const std::string &version, const bool requestEcho)
{
    bool result = true;
    if (!name.empty())
    {
        result &= SendRoute(Build(mMsgTemp, GATEWAY_ADDRESS, NODE_SENSOR_ID, C_INTERNAL, I_SKETCH_NAME, requestEcho).set(name.c_str()));
    }
    if (!version.empty())
    {
        result &= SendRoute(Build(mMsgTemp, GATEWAY_ADDRESS, NODE_SENSOR_ID, C_INTERNAL, I_SKETCH_VERSION, requestEcho).set(version.c_str()));
    }
    return result;
}

uint8_t MySensorsLFGateway::GetNodeId()
{
    uint8_t result;
//#if defined(MY_GATEWAY_FEATURE)
    result = GATEWAY_ADDRESS;
//#elif defined(MY_SENSOR_NETWORK)
//    result = transportGetNodeId();
//#else
//    result = VALUE_NOT_DEFINED;
//#endif
    return result;
}

char* MySensorsLFGateway::ProtocolMyMessage2Serial(const MyMessage &message)
{
    sprintf(mMessageBuffer, //(uint8_t)MY_GATEWAY_MAX_SEND_LENGTH,
            "%d;%d;%d;%d;%d;%s\n",
            message.getSender(),
            message.getSensor(),
            message.getCommand(),
            message.isEcho(),
            message.getType(),
            message.getString(_convBuffer));
    return mMessageBuffer;
}

bool MySensorsLFGateway::ProtocolSerial2MyMessage(MyMessage &message, char *inputString)
{
    char *str, *p;
    uint8_t index = 0;
    mysensors_command_t command = C_INVALID_7;
    message.setSender(GATEWAY_ADDRESS);
    message.setLast(GATEWAY_ADDRESS);
    message.setEcho(false);

    // Extract command data coming on serial line
    for (str = strtok_r(inputString, ";", &p); // split using semicolon
            str && index < 5; // loop while str is not null an max 4 times
            str = strtok_r(NULL, ";", &p), index++ // get subsequent tokens
        ) {
        switch (index) {
        case 0: // Radio id (destination)
            message.setDestination(atoi(str));
            break;
        case 1: // Child id
            message.setSensor(atoi(str));
            break;
        case 2: // Message type
            command = static_cast<mysensors_command_t>(atoi(str));
            message.setCommand(command);
            break;
        case 3: // Should we request echo from destination?
            message.setRequestEcho(atoi(str) ? 1 : 0);
            break;
        case 4: // Data type
            message.setType(atoi(str));
            break;
        }
    }
    // payload
    if (str == NULL) {
        // no payload, set default value
        message.set((uint8_t)0);
    } else if (command == C_STREAM) {
        // stream payload
        uint8_t bvalue[MAX_PAYLOAD_SIZE];
        uint8_t blen = 0;
        while (*str) {
            uint8_t val;
            val = convertH2I(*str++) << 4;
            val += convertH2I(*str++);
            bvalue[blen] = val;
            blen++;
        }
        message.set(bvalue, blen);
    } else {
        // regular payload
        char *value = str;
        // Remove trailing carriage return and newline character (if it exists)
        const uint8_t lastCharacter = strlen(value) - 1;
        if (value[lastCharacter] == '\r' || value[lastCharacter] == '\n') {
            value[lastCharacter] = '\0';
        }
        message.set(value);
    }
    return (index == 5);
}
