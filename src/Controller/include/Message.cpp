//
// Created by Tanner on 10/22/2023.
//

#include "Message.h"
extern AsyncWebSocket webserv;
extern AsyncWebServer server;



//TODO: add a way to send a message to a specific client
//TODO: Fix Broadcast to all clients



__attribute__((unused)) void sendToAll(const String &data) {
    webserv.textAll(data);
}

void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}