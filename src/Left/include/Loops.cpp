//
// Created by Tanner on 10/30/2023.
//

#include "Loops.h"
#include "Handlers.h"
#include "ArduinoWebsockets.h"


bool ackReceived = false;
unsigned long lastSent = millis() - 2000;
const unsigned long interval = 2000;
unsigned long lastHeartbeat = 0;
const unsigned long HEARTBEAT_INTERVAL = 60000;
unsigned long lastHeartbeatReceived = 0;
const unsigned long HEARTBEAT_TIMEOUT = 1000;
long heartbeatTimeoutCounter = 0;

using namespace websockets;

extern WebsocketsClient client;

void ackLoop(){
    if (!ackReceived && (millis() - lastSent >= interval)) {
        client.send(String("Left"));  // Send ID to server
        Serial.println("Broadcasting init to server...");
        Serial.println(String(ackReceived));
        Serial.println("Left");
        lastSent = millis();
    }
}

void heartBeatLoop(){
    if (ackReceived && millis() - lastHeartbeat > HEARTBEAT_INTERVAL) {
        sendHeartbeat();
        Serial.println("Sending heartbeat to server...");
        lastHeartbeat = millis();
    }
    if (lastHeartbeatReceived > HEARTBEAT_TIMEOUT) {
        lastHeartbeatReceived = millis();
        heartbeatTimeoutCounter++;
        ackReceived = false;
        Serial.println("Heartbeat failed... Retrying Ack");
    }
    if (heartbeatTimeoutCounter >= 1 ) {
        mute(1);
        delay(1000);
        mute(0);
        delay(750);
        mute(1);
        delay(1000);
        mute(0);
        delay(750);
        mute(1);
        Serial.println("2 heartbeat timeouts in a row.. Resetting server ack...");
        ackReceived = false;
    }
}

