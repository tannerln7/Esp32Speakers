//
// Created by Tanner on 10/30/2023.
//

#include "Handlers.h"

float muteState = 0.00;
float currentVolume = -10;
float currentSubVolume = 0;
float currentSource = 2.00;

extern SigmaDSP dsp;
extern WiFiClient espClient;
extern PubSubClient client;

extern unsigned long lastHeartbeatSent;
extern unsigned long lastHeartbeatReceived;
extern unsigned long heartbeatTimeoutCounter;
extern bool ackReceived;
extern const char* rightAckTopic;

void handleCallback(String data) {
    String command, value1, value2, value3, value4, value5, value6, value7;
    parseIncomingMessage(data, command, value1, value2, value3, value4, value5, value6, value7);
    Serial.println(
            "COMMAND: " + command + " Values: " + value1 + " " + value2 + " " + value3 + " " + value4 + " " + value5 +
            " " + value6 + " " + value7);
    if (command.equals("Volume")) {
        handleVolumeCommand(value1);
    } else if (command.equals("Sub")) {
        handleSubCommand(value1);
    } else if (command.equals("Mute")) {
        handleMuteCommand(value1);
    } else if (command.equals("Source")) {
        handleSourceCommand(value1);
    } else if (command.equals("heartBeatGOOD") || command.equals("heartBeatBAD")) {
        handleHeartbeatCommand(command, value1, value2, value3, value4, value5, value6, value7);
    } else if (command.equals("Ack")) {
        Serial.println("Ack received...");
        handleAckCommand(value1, value2, value3, value4, value5, value6, value7);
    } else {
        Serial.println("Unknown Command...");
    }
}

void handleVolumeCommand(String &value1){
    setVolume(value1.toFloat());
    String message = "VolumeACK:" + String(currentVolume);
    client.publish(rightAckTopic, message.c_str());
    Serial.println("Volume changed to " + String(currentVolume));
}
void handleSubCommand(String &value1){
    setSubVolume(value1.toFloat());  // Using value2
    String message = "SubACK:" + String(currentSubVolume);
    client.publish(rightAckTopic, message.c_str());
    Serial.print("Sub Volume Changed to " + String(currentSubVolume));
}
void handleMuteCommand(String &value1){
    mute(value1.toFloat());
    String message = "MuteACK:" + String(muteState);
    client.publish(rightAckTopic, message.c_str());
    Serial.println("Mute set to " + String(muteState));
}

void handleSourceCommand(String &value1){
    changeSource(value1.toFloat());
    String message = "SourceACK:" + String(currentSource);
    client.publish(rightAckTopic, message.c_str());
    Serial.println("Source changed to " + String(currentSource));
}

void handleHeartbeatCommand(String &command, String &value1, String &value2, String &value3, String &value4, String &value5, String &value6, String &value7){
    lastHeartbeatReceived = millis();
    if (command.equals("heartBeatGOOD")){
        ackReceived = true;
        lastHeartbeatSent = millis();
        heartbeatTimeoutCounter = 0;
        Serial.println("Heartbeat GOOD");
    }else if (command.equals("heartBeatBAD")){
        setVolume(value1.toFloat());
        setSubVolume(value2.toFloat());  // Assuming you've extracted value3 as well
        mute(value3.toFloat());
        changeSource(value4.toFloat());
        Serial.println("Heartbeat BAD!");
        Serial.println("Volume set to " + String(currentVolume));
        Serial.println("Sub Volume set to " + String(currentSubVolume));
        Serial.println("Mute set to " + String(muteState));
        Serial.println("Source set to " + String(currentSource));
    }
}

void handleAckCommand(String &value1, String &value2, String &value3, String &value4, String &value5, String &value6, String &value7){
    Serial.println("handleAckCommand Started...");
    Serial.println("Right: " " value1: " + value1 + " value2: " + value2 + " value3: " + value3 + " value4: " + value4 + " value5: " + value5 + " value6: " + value6 + " value7: " + value7);
    ackReceived = true;
    setVolume(value1.toFloat());
    setSubVolume(value2.toFloat());
    mute(value3.toFloat());
    changeSource(value4.toFloat());
    Serial.println("Server ack complete.");
    Serial.println("Volume: " + String(value1));
    Serial.println("Sub Volume: " + String(value2));
    Serial.println("Mute: " + String(value3));
    Serial.println("Source: " + String(value4));
}



void parseIncomingMessage(String &msg, String &command, String &value1, String &value2, String &value3, String &value4,
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


String getValue(const String& data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
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

void sendHeartbeat() {
    // Create the heartbeat message with current volume, sub levels, and mute state
    String heartbeatMessage = "HeartbeatRight:" + String(currentVolume) + ":" + String(currentSubVolume) + ":" + String(muteState) + ":" + String(currentSource);

    // Send the heartbeat message to the server
    client.publish(rightAckTopic, heartbeatMessage.c_str());

    // Log the heartbeat message
    Serial.println("Sent heartbeat message: " + heartbeatMessage);
}

void setVolume(float newVolume) {
    dsp.volume_slew(MOD_SWVOL1_ALG0_TARGET_ADDR, newVolume, 12);
    currentVolume = newVolume;
}

void mute(float newMuteState) {  // Toggle mute state
    if (String(newMuteState).equals("0.00")){
        dsp.mute(MOD_MUTE1_MUTENOSLEWALG1MUTE_ADDR, false);
    }else if (String(newMuteState).equals("1.00")){
        dsp.mute(MOD_MUTE1_MUTENOSLEWALG1MUTE_ADDR, true);
    }
    muteState = newMuteState;
}

void setSubVolume(float newSubVolume) {
    dsp.volume_slew(MOD_SWVOL2_ALG0_TARGET_ADDR, newSubVolume, 12);
    currentSubVolume = newSubVolume;
}

void changeSource(float newSource) {
    dsp.mux(MOD_NX2_1_STEREOSWSLEW_ADDR, static_cast<int>(newSource) - 1);
    currentSource = newSource;
}