//
// Created by Tanner on 10/22/2023.
//

#include "Message.h"

//TODO: add a way to send a message to a specific client
//TODO: Fix Broadcast to all clients

void sendCommandToClient(const String &command, const String &value) {

    String message = command + ":" + value;
    ws.textAll(message);
    lastMessageLeft = message;
    lastMessageRight = message;
    lastSendTimeLeft = millis();
    lastSendTimeRight = millis();
    retryCountLeft = 0;
    retryCountRight = 0;
}

void sendToAll(const String &data) {
    ws.textAll(data);
}

void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}