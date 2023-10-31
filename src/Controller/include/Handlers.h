//
// Created by Tanner on 10/22/2023.
//
#include <ArduinoWebsockets.h>
#include <cmath> // for std::fabs
#include <SharedVariables.h>
#include <map>
#include <Arduino.h>
#include <DEFINES.h>
#include <vector>

using namespace websockets;

void handleCallback(const String &msg);
void volume(float);
void sub(float);
void mute(int);
String getValue(const String &data, char separator, int index);
float mapLinearToDb(float);
void source(int);
void parseIncomingMessage(const String &msg, String &command, std::vector<String> &values);
void handleAck(const float &currentValue, const float &receivedValue);
bool floatEqual(float a, float b);
void handleHeartbeat(const std::vector<String> &values);
void handleInit();
void handleTime(const String &value1);
unsigned long abs_diff(unsigned long a, unsigned long b);
void handleIRCode(unsigned long code);
void webSocketSetup();
void onMessageCallback(const WebsocketsMessage& message);
void sendCommandToClient(const String& command, const String &value);