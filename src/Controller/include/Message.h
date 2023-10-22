//
// Created by Tanner on 10/22/2023.
//

#ifndef ESP32SPEAKERS_MESSAGE_H
#define ESP32SPEAKERS_MESSAGE_H

#endif //ESP32SPEAKERS_MESSAGE_H

#include <map>
#include "AsyncWebSocket.h"
#include <Handlers.h>


void sendCommandToClient(const String &command, const String &value);
void sendToAll(const String &data);
void sendMessage();
void onMessage(const AsyncWebSocketMessage& message);
void onConnection(AsyncWebSocketClient& client, const String& message);
void onDisconnection();
void nodeTimeAdjustedCallback(int32_t offset);
