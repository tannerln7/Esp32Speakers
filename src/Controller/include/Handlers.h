//
// Created by Tanner on 10/22/2023.
//
#include "AsyncWebSocket.h"
#include <cmath> // for std::fabs
#include <SharedVariables.h>
#include <map>
#include <Arduino.h>
#include <IRrecv.h>
#include <DEFINES.h>

#pragma once

void handleCallback(const uint32_t &from, const String &msg);
void volume(float);
void sub(float);
void mute(int);
String getValue(const String &data, char separator, int index);
float mapLinearToDb(float);
void source(int);
void parseIncomingMessage(const String &msg, String &command, String &value1, String &value2, String &value3, String &value4,
                          String &value5, String &value6, String &value7);
void handleAck(uint32_t clientID, const float &currentValue, const float &receivedValue);
bool floatEqual(float a, float b);
void handleHeartbeat(uint32_t from, const String &value1, const String &value2, const String &value3, const String &value4);
void handleInit(const uint32_t &from, const String &command);
void handleTime(uint32_t from, const String &value1);
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);
void sendCommandToClient(const String &command, const String &value);

[[noreturn]] void IRReceiverTask(void * pvParameters);
void handleIRCode(uint32_t code);
void irSetup();