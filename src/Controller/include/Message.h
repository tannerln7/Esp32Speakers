//
// Created by Tanner on 10/22/2023.
//

#ifndef ESP32SPEAKERS_MESSAGE_H
#define ESP32SPEAKERS_MESSAGE_H

#endif //ESP32SPEAKERS_MESSAGE_H

#include <map>
#include <SharedVariables.h>


__attribute__((unused)) void sendToAll(const String &data);
void sendMessage();

//__attribute__((unused)) void onMessage(const AsyncWebSocketMessage& message);
//void onConnection(AsyncWebSocketClient& client, const String& message);
//void onDisconnection();
//void nodeTimeAdjustedCallback(int32_t offset);
