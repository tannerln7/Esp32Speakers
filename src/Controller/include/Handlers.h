//
// Created by Tanner on 10/22/2023.
//
#include "AsyncWebSocket.h"
#include <cmath> // for std::fabs
#include <SharedVariables.h>
#include <map>
#include <Arduino.h>
#include <DEFINES.h>

#pragma once

void handleCallback(const uint32_t &from, const String &msg);
void volume(float);
void sub(float);
void mute(int);
String getValue(const String &data, char separator, int index);
float mapLinearToDb(float);
void source(int);
void parseIncomingMessage(const String &msg, String &command, std::vector<String> &values);
void handleAck(uint32_t clientID, const float &currentValue, const float &receivedValue);
bool floatEqual(float a, float b);
void handleHeartbeat(const uint32_t &from, const std::vector<String> &values);
void handleInit(const uint32_t &from, const String &command);
void handleTime(uint32_t from, const String &value1);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void sendCommandToClient(const String &command, const String &value);
unsigned long abs_diff(unsigned long a, unsigned long b);
void handleIRCode(unsigned long code);
void webSocketSetup();