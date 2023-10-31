//
// Created by Tanner on 10/30/2023.
//

#ifndef ESP32SPEAKERS_WEBSOCKETSETUP_H
#define ESP32SPEAKERS_WEBSOCKETSETUP_H
#endif //ESP32SPEAKERS_WEBSOCKETSETUP_H

#include <WiFi.h>
#include "Handlers.h"
#include <ArduinoWebsockets.h>
#include <Handlers.h>

using namespace websockets;

void webSocketSetup();
void onEventsCallback(WebsocketsEvent event, const String& data);
void onMessageCallback(const WebsocketsMessage& message);

