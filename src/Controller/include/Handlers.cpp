//
// Created by Tanner on 10/22/2023.
//

#include "Message.h"
#include "DEFINES.h"


float mapLinearToDb(float linearVolume) {
    return (linearVolume - MIN_LINEAR_VOLUME) * (MAX_DB_VOLUME - MIN_DB_VOLUME) /
           (MAX_LINEAR_VOLUME - MIN_LINEAR_VOLUME) + MIN_DB_VOLUME;
}


void handleCallback(const uint32_t &from, const String &msg) {
    String command, value1, value2, value3, value4, value5, value6, value7;
    parseIncomingMessage(msg, command, value1, value2, value3, value4, value5, value6, value7);
    Serial.println("COMMAND VALUES " + command + " " + value1 + " " + value2 + " " + value3 + " " + value4);
    std::map<String, float *> commandToVariable = {
            {"volumeACK",    &currentVolumeDb},
            {"subVolumeAck", &currentSubVolumeDb},
            {"muteACK",      reinterpret_cast<float *>(&muteState)},
            {"sourceACK",    reinterpret_cast<float *>(&currentSource)}
    };

    auto it = commandToVariable.find(command);
    if (it != commandToVariable.end()) {
        float *variableToPass = it->second;
        float receivedValue = value1.toFloat();
        handleAck(from, *variableToPass, receivedValue);
    } else if (command.equals("time")) {
        handleTime(from, value1);
    } else if (command.equals("init")) {
        handleInit(from, value1);
    }
    else if (command.equals("heartBeat")) {
        handleHeartbeat(from, value1, value2, value3, value4);
    }
}

unsigned long abs_diff(unsigned long a, unsigned long b) {
    return (a > b) ? (a - b) : (b - a);
}

void handleAck(const uint32_t clientID, const float &currentValue, const float &receivedValue) {
    bool isSame = floatEqual(currentValue, receivedValue);

    if (clientID == leftId){
        leftAck = isSame;
    } else if (clientID == rightId) {
        rightAck = isSame;
    }
}

bool floatEqual(float a, float b) {
    float tolerance = 0.0001;
    return std::fabs(a - b) < tolerance;
}


void handleTime(uint32_t from, const String &value1) {
    unsigned long currentTimeMillis = millis();
    unsigned long receivedTime = value1.toInt();
    bool isTimeWithinTolerance = abs_diff(currentTimeMillis, receivedTime) <= TIME_TOLERANCE;
    String currentTime = String(currentTimeMillis);

    if (from == leftId) {
        leftTime = isTimeWithinTolerance;
    } else if (from == rightId) {
        rightTime = isTimeWithinTolerance;
    }
    if (isTimeWithinTolerance) {
        Serial.println("Time is synced with " + String(from) + " at " + currentTime);
    } else {
        Serial.println(String(from) + ": time not equal. Current time: " + currentTime + ", Received time: " + value1);
    }
}

void handleInit(const uint32_t &from, const String &command) {
    //TODO:Debug client id initialization
    if (command.equals("Left") && !leftInit) {
        leftId = from;
        leftInit = true;
        ws.text(leftId,
                "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                ":" + String(currentSource));
        Serial.println("Left Init complete!");
    } else if (command.equals("Right") && !rightInit) {
        rightId = from;
        rightInit = true;
        ws.text(rightId,
                "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                ":" + String(currentSource));
        Serial.println("Right Init complete!");
    }
}

void handleHeartbeat(uint32_t from, const String &value1, const String &value2, const String &value3, const String &value4) {
    if (value1.equals(String(currentVolumeDb)) && value2.equals(String(currentSubVolumeDb)) &&
        value3.equals(String(muteState)) && value4.equals(String(currentSource))) {
        ws.text(from, "heartBeatGOOD");
        Serial.println(String(from) + ": Heartbeat GOOD");
    } else {
        ws.text(leftId, "heartBeatBAD:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" +
                        String(muteState) + ":" + String(currentSource));
        Serial.println(String(from) + ": Heartbeat BAD");
        Serial.println(String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) + ":" +
                       String(currentSource));
    }
}


void parseIncomingMessage(const String &msg, String &command, String &value1, String &value2, String &value3, String &value4,
                          String &value5, String &value6, String &value7) {
    command = getValue(msg, ':', 0);
    value1 = getValue(msg, ':', 1);
    value2 = getValue(msg, ':', 2);
    value3 = getValue(msg, ':', 3);
    value4 = getValue(msg, ':', 4);
    value5 = getValue(msg, ':', 5);
    value6 = getValue(msg, ':', 6);
    value7 = getValue(msg, ':', 7);
}
//TODO:Replace implement WebSockets instead of sendCommandToClient

void volume(float newLinearVolume) {
    currentVolumeDb = mapLinearToDb(newLinearVolume);
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Volume", String(currentVolumeDb));
    currentLinearVolume = newLinearVolume;
    Serial.print("Volume UP: ");
    Serial.println(String(currentVolumeDb) + "Db");
}

void sub(float newLinearSubVolume) {
    currentSubVolumeDb = mapLinearToDb(newLinearSubVolume);
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Sub", String(currentSubVolumeDb));
    currentLinearSubVolume = newLinearSubVolume;
}

void mute(int newMuteState) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Mute", String(newMuteState));
    muteState = newMuteState;
}

void source(int newSource) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Source", String(newSource));
    currentSource = newSource;
}

String getValue(const String &data, char separator, int index) {
    int found = 0;
    int strIndex[] = {0, -1};
    uint maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }

    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void onWsEvent(AsyncWebSocket *serverName, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {

    String receivedMsg = String((char *) data).substring(0, len);

    if (type == WS_EVT_CONNECT) {
        Serial.println("Websocket client connection received : " + String(client->id()));
    }else if (type == WS_EVT_DISCONNECT) {
        Serial.println("Websocket client connection closed : " + String(client->id()));
    } else if (type == WS_EVT_DATA) {
        handleCallback(client->id(), receivedMsg);
    }
}