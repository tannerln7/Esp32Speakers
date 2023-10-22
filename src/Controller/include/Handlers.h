//
// Created by Tanner on 10/22/2023.
//
#include "AsyncWebSocket.h"
#include <unordered_map>
#include <cmath> // for std::fabs
#include <SharedVariables.h>

#ifdef INCLUDE_METHOD_MAP_LINEAR_TO_DB
float mapLinearToDb(float);
#endif




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