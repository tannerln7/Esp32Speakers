//
// Created by Tanner on 10/22/2023.
//

#include <Handlers.h>
#include <ArduinoWebsockets.h>

float currentLinearVolume = START_VOLUME;
float currentVolumeDb = mapLinearToDb(START_VOLUME);
float currentLinearSubVolume = START_SUB_VOLUME;
float currentSubVolumeDb = mapLinearToDb(START_SUB_VOLUME);
int muteState = START_MUTE;
int currentSource = START_SOURCE;
bool recheck = true;

using namespace websockets;

extern WebsocketsServer wsServer;
extern WebsocketsClient wsClient;


float mapLinearToDb(float linearVolume) {
    return (linearVolume - MIN_LINEAR_VOLUME) * (MAX_DB_VOLUME - MIN_DB_VOLUME) /
           (MAX_LINEAR_VOLUME - MIN_LINEAR_VOLUME) + MIN_DB_VOLUME;
}


void handleCallback(const String &msg) {
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
        handleAck(*variableToPass, receivedValue);
    } else if (command.equals("time")) {
        handleTime(values[0]);
    } else if (command.equals("Left") || command.equals("Right")) {
        handleInit();
    } else if (command.equals("heartBeat")) {
        handleHeartbeat(values);
    }else {
        Serial.println("Error: Unknown command");
    }
}

unsigned long abs_diff(unsigned long a, unsigned long b) {
    return (a > b) ? (a - b) : (b - a);
}

void handleAck(const float &currentValue, const float &receivedValue) {
    bool isSame = floatEqual(currentValue, receivedValue);
    if (isSame) {

    } else {
        Serial.println("Error: " + String(currentValue) + " does not equal " + String(receivedValue));
    }
    //TODO: figure this out....
}

bool floatEqual(float a, float b) {
    float tolerance = 0.0001;
    return std::fabs(a - b) < tolerance;
}


void handleTime(const String &value1) {
    unsigned long currentTimeMillis = millis();
    unsigned long receivedTime = value1.toInt();
    bool isTimeWithinTolerance = abs_diff(currentTimeMillis, receivedTime) <= TIME_TOLERANCE;
    String currentTime = String(currentTimeMillis);

    if (isTimeWithinTolerance) {
        Serial.println("Time is synced at " + currentTime);
    } else {
        Serial.println("time not equal. Current time: " + currentTime + ", Received time: " + value1);
        wsClient.send("time:" + String(millis()));
    }
}

void handleInit() {
        wsClient.send(
                "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
                ":" + String(currentSource));
        Serial.println("Init complete!");
}

void handleHeartbeat(const std::vector<String> &values) {
    String value1 = !values.empty() ? values[0] : "";
    String value2 = values.size() > 1 ? values[1] : "";
    String value3 = values.size() > 2 ? values[2] : "";
    String value4 = values.size() > 2 ? values[3] : "";
    if (value1.equals(String(currentVolumeDb)) && value2.equals(String(currentSubVolumeDb)) &&
        value3.equals(String(muteState)) && value4.equals(String(currentSource))) {
        wsClient.send("heartBeatGOOD");
        Serial.println("Heartbeat GOOD");
    } else {
        wsClient.send("heartBeatBAD:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" +
                        String(muteState) + ":" + String(currentSource));
        Serial.println("Heartbeat BAD");
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
    ack = false;
    sendCommandToClient("Volume", String(currentVolumeDb));
    currentLinearVolume = newLinearVolume;
    Serial.print("Sent Volume: ");
    Serial.println(String(currentVolumeDb) + "Db");
}

void sub(float newLinearSubVolume) {
    currentSubVolumeDb = mapLinearToDb(newLinearSubVolume);
    ack = false;
    sendCommandToClient("Sub", String(currentSubVolumeDb));
    currentLinearSubVolume = newLinearSubVolume;
    Serial.print("Sent Sub: ");
    Serial.println(String(currentSubVolumeDb) + "Db");
}

void mute(int newMuteState) {
    ack = false;
    sendCommandToClient("Mute", String(newMuteState));
    muteState = newMuteState;
    Serial.print("Sent Mute: ");
    Serial.println(String(muteState));
}

void source(int newSource) {
    ack = false;
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

void onWsEvent(WebsocketsEvent event, const String& data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void onMessageCallback(const WebsocketsMessage& message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    handleCallback(message.data());
}

void webSocketSetup() {
    // Start WebSocket server
    wsServer.listen(8080);
    Serial.println("WebSocket server started on " + String(WiFi.localIP()) + ":" + String(8080));

    // Connect to WebSocket server
    if(wsClient.connect("ws://192.168.1.248:8080")) {
        Serial.println("Connected to WebSocket server: ws://192.168.1.248:8080");
    } else {
        Serial.println("Failed to connect to WebSocket server");
    }

    // Setup Callbacks
    wsClient.onMessage(onMessageCallback);
    wsClient.onEvent(onWsEvent);
}

void sendCommandToClient(const String& command, const String &value) {

    String message = command + ":" + value;
    wsClient.send(message);
    lastMessage = message;
    lastSendTime = millis();
    retryCount = 0;
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