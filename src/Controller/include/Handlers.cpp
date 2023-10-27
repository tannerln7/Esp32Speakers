//
// Created by Tanner on 10/22/2023.
//

#include <Handlers.h>

float currentLinearVolume = START_VOLUME;
float currentVolumeDb = mapLinearToDb(START_VOLUME);
float currentLinearSubVolume = START_SUB_VOLUME;
float currentSubVolumeDb = mapLinearToDb(START_SUB_VOLUME);
int muteState = START_MUTE;
int currentSource = START_SOURCE;
String lastMessageLeft = "";
String lastMessageRight = "";
unsigned long lastSendTimeLeft = 0;
unsigned long lastSendTimeRight = 0;
int retryCountLeft = 0;
int retryCountRight = 0;
uint32_t leftId = 0;
uint32_t rightId = 0;
bool leftAck = false;
bool rightAck = false;
bool leftInit = false;
bool rightInit = false;
bool recheck = true;
bool leftTime;
bool rightTime;

extern AsyncWebServer server;
extern AsyncWebSocket webserv;

float mapLinearToDb(float linearVolume) {
    return (linearVolume - MIN_LINEAR_VOLUME) * (MAX_DB_VOLUME - MIN_DB_VOLUME) /
           (MAX_LINEAR_VOLUME - MIN_LINEAR_VOLUME) + MIN_DB_VOLUME;
}


void handleCallback(const uint32_t &from, const String &msg) {
    String command;
    std::vector<String> values;
    parseIncomingMessage(msg, command, values);

    std::map<String, float *> commandToVariable = {
            {"volumeACK",    &currentVolumeDb},
            {"subVolumeAck", &currentSubVolumeDb},
            {"muteACK",      reinterpret_cast<float *>(&muteState)},
            {"sourceACK",    reinterpret_cast<float *>(&currentSource)}
    };

    auto it = commandToVariable.find(command);

    if (it != commandToVariable.end()) {
        float *variableToPass = it->second;
        float receivedValue = values[0].toFloat();
        handleAck(from, *variableToPass, receivedValue);
    } else if (command.equals("time")) {
        handleTime(from, values[0]);
    } else if (command.equals("init")) {
        handleInit(from, values[0]);
    } else if (command.equals("heartBeat")) {
        handleHeartbeat(from, values);
    } else {
        Serial.println("Error: Unknown command");
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
        webserv.text(leftId,
                "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                ":" + String(currentSource));
        Serial.println("Left Init complete!");
    } else if (command.equals("Right") && !rightInit) {
        rightId = from;
        rightInit = true;
        webserv.text(rightId,
                "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                ":" + String(currentSource));
        Serial.println("Right Init complete!");
    }
}

void handleHeartbeat(const uint32_t &from, const std::vector<String> &values) {
    String value1 = !values.empty() ? values[0] : "";
    String value2 = values.size() > 1 ? values[1] : "";
    String value3 = values.size() > 2 ? values[2] : "";
    String value4 = values.size() > 2 ? values[3] : "";
    if (value1.equals(String(currentVolumeDb)) && value2.equals(String(currentSubVolumeDb)) &&
        value3.equals(String(muteState)) && value4.equals(String(currentSource))) {
        webserv.text(from, "heartBeatGOOD");
        Serial.println(String(from) + ": Heartbeat GOOD");
    } else {
        webserv.text(leftId, "heartBeatBAD:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" +
                        String(muteState) + ":" + String(currentSource));
        Serial.println(String(from) + ": Heartbeat BAD");
        Serial.println(String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) + ":" +
                       String(currentSource));
    }
}


void parseIncomingMessage(const String &msg, String &command, std::vector<String> &values) {
    command = getValue(msg, ':', 0);
    int index = 1;
    String value;

    while ((value = getValue(msg, ':', index)) != "") {
        values.push_back(value);
        ++index;
    }
}
//TODO:Replace implement WebSockets instead of sendCommandToClient

void volume(float newLinearVolume) {
    currentVolumeDb = mapLinearToDb(newLinearVolume);
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Volume", String(currentVolumeDb));
    currentLinearVolume = newLinearVolume;
    Serial.print("Sent Volume: ");
    Serial.println(String(currentVolumeDb) + "Db");
}

void sub(float newLinearSubVolume) {
    currentSubVolumeDb = mapLinearToDb(newLinearSubVolume);
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Sub", String(currentSubVolumeDb));
    currentLinearSubVolume = newLinearSubVolume;
    Serial.print("Sent Sub: ");
    Serial.println(String(currentSubVolumeDb) + "Db");
}

void mute(int newMuteState) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Mute", String(newMuteState));
    muteState = newMuteState;
    Serial.print("Sent Mute: ");
    Serial.println(String(muteState));
}

void source(int newSource) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient("Source", String(newSource));
    currentSource = newSource;
    Serial.print("Sent Source: ");
    Serial.println(String(currentSource));
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

void webSocketSetup() {
    server.addHandler(&webserv);
    webserv.onEvent(onWsEvent);
    Serial.println("Websocket Setup Complete");
}

void sendCommandToClient(const String &command, const String &value) {

    String message = command + ":" + value;
    webserv.textAll(message);
    lastMessageLeft = message;
    lastMessageRight = message;
    lastSendTimeLeft = millis();
    lastSendTimeRight = millis();
    retryCountLeft = 0;
    retryCountRight = 0;
}

const unsigned long volumeUpCode = 0xE21DEF00;
const unsigned long volumeDownCode = 0xE51AEF00;
const unsigned long muteCode = 0xFC03EF00;
const unsigned long subUpCode = 0xE31CEF00;
const unsigned long subDownCode = 0xE11EEF00;
const unsigned long sourceCode = 0xEB14EF00;
const unsigned long repeatCode = 0;
unsigned long lastIRCode = 0;

void handleIRCode(unsigned long code) {
    Serial.println(code, HEX);
    do {
        recheck = false;
        switch (code) {
            case volumeUpCode:
                lastIRCode = code;
                if (currentLinearVolume < MAX_LINEAR_VOLUME) {
                    currentLinearVolume++;
                    volume(currentLinearVolume);
                    Serial.println("Volume Up: " + String(currentLinearVolume));
                } else {
                    volume(currentLinearVolume);
                    Serial.println("Volume already at max.");
                }
                break;
            case volumeDownCode:
                lastIRCode = code;
                if (currentLinearVolume > MIN_LINEAR_VOLUME) {
                    currentLinearVolume--;
                    volume(currentLinearVolume);
                    Serial.println("Volume Down: " + String(currentLinearVolume));
                } else {
                    volume(currentLinearVolume);
                    Serial.println("Volume already at min");
                }
                break;
            case muteCode:
                lastIRCode = code;
                if (muteState == 0) {
                    mute(1);
                    muteState = 1;
                    Serial.println("Mute ON");
                } else {
                    mute(0);
                    muteState = 0;
                    Serial.println("Mute OFF");
                }
                break;
            case subUpCode:
                lastIRCode = code;
                if (currentLinearSubVolume < MAX_LINEAR_VOLUME) {
                    currentLinearSubVolume++;
                    sub(currentLinearSubVolume);
                    Serial.println("Sub Volume Up: " + String(currentLinearSubVolume));
                } else {
                    sub(currentLinearSubVolume);
                    Serial.println("Sub Volume already at max");
                }
                break;
            case subDownCode:
                lastIRCode = code;
                if (currentLinearSubVolume > MIN_LINEAR_VOLUME) {
                    currentLinearSubVolume--;
                    sub(currentLinearSubVolume);
                    Serial.println("Sub Volume Down: " + String(currentLinearSubVolume));
                } else {
                    sub(currentLinearSubVolume);
                    Serial.println("Sub volume already at min");
                }
                break;
            case sourceCode:
                lastIRCode = code;
                if (currentSource == 1) {
                    source(2);
                    Serial.println("Changed to source 2 (Line in)");
                } else if (currentSource == 2) {
                    source(1);
                    Serial.println("Changed to source 1 (Optical)");
                }
                break;
            case repeatCode:
                if (lastIRCode != 0) {
                    code = lastIRCode;
                    recheck = true;
                }
                Serial.println("Repeat Code:" + String(code, HEX));
                break;
            default:
                Serial.println("Unknown Code");
                break;
        }
    }while (recheck);
}