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
uint32_t myID = 0;
uint32_t leftId = 0;
uint32_t rightId = 0;
bool leftAck = false;
bool rightAck = false;
bool leftTime = false;
bool rightTime = false;
bool leftInit = false;
bool rightInit = false;
const unsigned long debounceDelay = DEBOUNCE_DELAY;



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

void handleHeartbeat(uint32_t from, const String &value1, const String &value2, const String &value3, const String &value4) {
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

void webSocketSetup() {
    webserv.onEvent(onWsEvent);
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

extern IRrecv irrecv;
extern decode_results results;

const uint32_t volumeUpCode = 0xF7B847;
const uint32_t volumeDownCode = 0xF758A7;
const uint32_t muteCode = 0xF7C03F;
const uint32_t subUpCode = 0xF738C7;
const uint32_t subDownCode = 0xF77887;
const uint32_t sourceCode = 0xF728D7;
const uint64_t repeatCode = 0xFFFFFFFFFFFFFF;

unsigned long lastIRReceivedTime = 0;
long lastOperation = 0;

void handleIRCode(uint32_t code) {
    Serial.println(code, HEX);
    switch (code) {
        case volumeUpCode:
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
            if (currentSource == 1) {
                source(2);
                Serial.println("Changed to source 2 (Line in)");
            } else if (currentSource == 2) {
                source(1);
                Serial.println("Changed to source 1 (Optical)");
            }
            break;
        default:
            Serial.println("Unknown Code");
            break;
    }
}

[[noreturn]] void IRReceiverTask(void * pvParameters) {
    // Initialize your IR receiver here
    while (true) {
        if (irrecv.decode(&results)) {
            unsigned long currentIRReceivedTime = millis();
            // If it's a repeat code, replay the last operation
            if (results.value == repeatCode && lastOperation != 0) {
                if (currentIRReceivedTime - lastIRReceivedTime > debounceDelay) {
                    handleIRCode(lastOperation);
                    Serial.println("Repeat Code" + String(lastOperation));
                }
            } else {
                // If it's not a repeat code and enough time has passed (debouncing)
                if (currentIRReceivedTime - lastIRReceivedTime > debounceDelay) {
                    lastOperation = (long) results.value;
                    handleIRCode(results.value);
                    Serial.println(results.value, HEX);
                }
            }
            lastIRReceivedTime = currentIRReceivedTime;
            irrecv.resume();
        }
        // Use delay to free up the core for other tasks
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void irSetup() {
    xTaskCreatePinnedToCore(
            IRReceiverTask,      /* Task function */
            "IRReceiverTask",    /* Task name */
            2000,               /* Stack size */
            nullptr,                /* Parameters */
            1,                   /* Priority */
            nullptr,                /* Task handle (if you want to reference it later, otherwise NULL) */
            1                    /* Core you want to run the task on, 0 or 1. 1 for the second core */
    );
}