//
// Created by Tanner on 10/30/2023.
//

#include "Loops.h"
#include "Handlers.h"


bool ackReceived = false;
unsigned long lastSent = millis() - 2000;
const unsigned long interval = 2000;
unsigned long lastHeartbeatSent = 0;
unsigned long lastHeartbeatReceived = 0;
const unsigned long HEARTBEAT_INTERVAL = 10000;
const unsigned long HEARTBEAT_TIMEOUT = 15000;
unsigned long heartbeatTimeoutCounter = 0;

extern WiFiClient espClient;
extern PubSubClient client;
extern const char* leftAckTopic;
const String leftId = "1111";


void ackLoop(){
    if (!ackReceived && (millis() - lastSent >= interval)) {
        String ackString = "Left:" + String(leftId);
        client.publish(leftAckTopic, ackString.c_str());  // Send ID to server
        Serial.println("Broadcasting init to server...");
        Serial.println(String(ackReceived));
        Serial.println((String)leftAckTopic + " Left:" + leftId);
        lastSent = millis();
    }
}

void heartBeatLoop() {
    unsigned long currentMillis = millis();

    // Time to send a new heartbeat
    if (ackReceived && currentMillis - lastHeartbeatSent >= HEARTBEAT_INTERVAL) {
        sendHeartbeat();
        Serial.println("Sending heartbeat to server...");
        lastHeartbeatSent = currentMillis;
    }

    // Check for heartbeat timeout
    if (currentMillis - lastHeartbeatSent >= HEARTBEAT_INTERVAL &&
        currentMillis - lastHeartbeatReceived > HEARTBEAT_TIMEOUT) {
        Serial.println("Heartbeat failed... Retrying Ack");
        lastHeartbeatReceived = currentMillis;
        heartbeatTimeoutCounter++;
        ackReceived = false;
    }

    // Action for two consecutive heartbeat timeouts
    if (heartbeatTimeoutCounter >= 2) {
        Serial.println("2 heartbeat timeouts in a row.. Resetting server ack...");
        mute(1);
        delay(1000);
        mute(0);
        delay(750);
        mute(1);
        delay(1000);
        mute(0);
        delay(750);
        mute(1);
        ackReceived = false;
        heartbeatTimeoutCounter = 0; // Reset the counter
    }
}


