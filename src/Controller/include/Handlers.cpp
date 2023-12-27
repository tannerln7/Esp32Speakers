//
// Created by Tanner on 10/22/2023.
//

#include <Handlers.h>

#define MAX_RETRY_COUNT 5
float muteState = START_MUTE;
float currentSource = START_SOURCE;


String leftLastMessage = "";
String rightLastMessage = "";
unsigned long rightRetryCount = 0;
unsigned long leftRetryCount = 0;
bool leftAck = false;
bool rightAck = false;
bool recheck = true;
extern bool leftInit;
extern bool rightInit;

extern const char* leftTopic;
extern const char* leftAckTopic;
extern const char* rightTopic;
extern const char* rightAckTopic;
const char* timeTopic = "home/livingroom/time";

extern WiFiClient espClient;
extern PubSubClient client;

// Global variable for slider position
double sliderPosition = -50.0; // Starting from 0 (silent)
double subSliderPosition = -50.0;
float currentVolumeDb;
float currentSubVolumeDb;
// Constants for the exponential curve
const double a = 0.001;
const double b = 6.908;

// Volume control function
double volumeControl(double position) {
    // Clamp slider position to [0, 1]
    sliderPosition = max(min(position, 1.0), 0.0);

    // Apply linear roll-off to zero for positions less than 0.1
    if (sliderPosition < 0.1) {
        return sliderPosition * 10 * a * exp(0.1 * b);
    }

    // Use exponential curve for positions 0.1 and above
    return a * exp(b * sliderPosition);
}

void handleCallback(String topic, String &msg) {
    String command;
    std::vector<String> values;
    parseIncomingMessage(msg, command, values);

    // Check if values is empty and exit if true
    if (values.empty()) {
        Serial.println("Error: Values is empty");
        return;
    }

    std::map<String, float *> commandToVariable = {
            {"VolumeACK",    &currentVolumeDb},
            {"SubACK", &currentSubVolumeDb},
            {"Mute",      &muteState},
            {"SourceACK",    &currentSource}
    };

    auto it = commandToVariable.find(command);
    if (it != commandToVariable.end()) {
        float *variableToPass = it->second;
        float receivedValue = values[0].toFloat();
        handleAck(topic, *variableToPass, receivedValue);
    } else {
        // Handles other cases that are not in the map
        if (command.equals("time")) {
            handleTime(topic, values[0]);
        } else if (command.equals("Left") || command.equals("Right")) {
            handleInit(topic, values);
        } else if (command.equals("HeartbeatLeft") || command.equals("HeartbeatRight")) {
            Serial.println(command);
            handleHeartbeat(topic, values);
        } else {
            Serial.println("Error: Unknown command");
        }
    }
}

void handleAck(String &topic, float &currentValue, const float &receivedValue) {
    bool isSame = (String(currentValue) == String(receivedValue));
    if (isSame) {
        if (topic.equals(leftAckTopic)) {
            leftAck = true;
            leftRetryCount = 0;
            Serial.println("Left ACK");
        } else if (topic.equals(rightAckTopic)) {
            rightAck = true;
            rightRetryCount = 0;
            Serial.println("Right ACK");
        }
    } else {
        Serial.println("Error: " + topic + ": " + String(currentValue) + " does not equal " + String(receivedValue));
        if (topic.equals(leftAckTopic) && leftRetryCount < MAX_RETRY_COUNT && !leftAck) {
            leftRetryCount++;
            client.publish(leftTopic, leftLastMessage.c_str());

        } else if (topic.equals(rightAckTopic) && rightRetryCount < MAX_RETRY_COUNT && !rightAck) {
            rightRetryCount++;
            client.publish(rightTopic, rightLastMessage.c_str());
        } else {
            Serial.println("Error: " + String(topic) + ": Max retry count reached");
        }
    }
}


void handleTime(String &topic, String &value1) {
    unsigned long currentTimeMillis = millis();
    unsigned long receivedTime = value1.toInt();
}

void handleInit(String &topic, std::vector<String> &values) {
    String command = values[0];
    String message = "Ack:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" + String(muteState) +
    ":" + String(currentSource);
    client.publish(getTopic(topic), message.c_str());
    Serial.println((String) getTopic(topic) + ": Init complete!");
}

void handleHeartbeat(String &topic, std::vector<String> &values) {
    String value1 = !values.empty() ? values[0] : "";
    String value2 = values.size() > 1 ? values[1] : "";
    String value3 = values.size() > 2 ? values[2] : "";
    String value4 = values.size() > 3 ? values[3] : "";
    if (value1.equals(String(currentVolumeDb)) && value2.equals(String(currentSubVolumeDb)) &&
        value3.equals(String(muteState)) && value4.equals(String(currentSource))) {
        client.publish(getTopic(topic), "HeartBeatGood");
        Serial.println(topic + "Heartbeat GOOD");
    } else {
        String badMessage = "heartBeatBAD:" + String(currentVolumeDb) + ":" + String(currentSubVolumeDb) + ":" +
                                String(muteState) + ":" + String(currentSource);
        client.publish(getTopic(topic), badMessage.c_str());
        Serial.println(topic + ": Heartbeat BAD");
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

void volume(float newVolume) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient(VOLUME_COMMAND, String(newVolume));
    Serial.print("Sent Volume: ");
    currentVolumeDb = newVolume;
    Serial.println(String(newVolume) + "Db");
}

void sub(float newSubVolume) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient(SUB_VOLUME_COMMAND, String(newSubVolume));
    Serial.print("Sent Sub: ");
    currentSubVolumeDb = newSubVolume;
    Serial.println(String(newSubVolume) + "Db");
}

void mute(float newMuteState) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient(MUTE_COMMAND, String(newMuteState));
    muteState = newMuteState;
    Serial.print("Sent Mute: ");
    Serial.println(String(muteState));
}

void source(float newSource) {
    leftAck = false;
    rightAck = false;
    sendCommandToClient( SOURCE_COMMAND, String(newSource));
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

void sendCommandToClient(const String& command, const String &value) {
    String message = command + ":" + value;
    client.publish(leftTopic, message.c_str());
    client.publish(rightTopic, message.c_str());
    leftLastMessage = message;
    rightLastMessage = message;
    rightRetryCount = 0;
    leftRetryCount = 0;
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
                sliderPosition += 0.04; // Increment by 1/25
                sliderPosition = max(min(sliderPosition, 1.0), 0.0); // Clamp to [0, 1]
                Serial.print("Volume increased to ");
                Serial.println(volumeControl(sliderPosition), 6);
                volume((float) volumeControl(subSliderPosition));
                break;
            case volumeDownCode:
                lastIRCode = code;
                sliderPosition -= 0.04; // Decrement by 1/25
                sliderPosition = max(min(sliderPosition, 1.0), 0.0); // Clamp to [0, 1]
                Serial.print("Volume decreased to ");
                Serial.println(volumeControl(sliderPosition), 6);
                volume((float) volumeControl(subSliderPosition));
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
                subSliderPosition += 0.04; // Increment by 1/25
                subSliderPosition = max(min(subSliderPosition, 1.0), 0.0); // Clamp to [0, 1]
                Serial.print("Volume increased to ");
                Serial.println(volumeControl(subSliderPosition), 6);
                sub((float) volumeControl(subSliderPosition));
                break;
            case subDownCode:
                lastIRCode = code;
                subSliderPosition -= 0.04; // Decrement by 1/25
                subSliderPosition = max(min(subSliderPosition, 1.0), 0.0); // Clamp to [0, 1]
                Serial.print("Volume decreased to ");
                Serial.println(volumeControl(subSliderPosition), 6);
                volume((float) volumeControl(subSliderPosition));
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

const char* getTopic(const String& topic) {
    if (topic.equals(leftAckTopic)) {
        return leftTopic;
    } else if (topic.equals(rightAckTopic)) {
        return rightTopic;
    } else {
        return nullptr;
    }
}