//
// Created by Tanner on 10/22/2023.
//
#include <WiFi.h>
#include "PubSubClient.h"
#include <cmath> // for std::fabs
#include <SharedVariables.h>
#include <map>
#include <Arduino.h>
#include <DEFINES.h>
#include <vector>

void handleCallback(String topic, String &msg);
void volume(float);
void sub(float);
void mute(float);
String getValue(const String &data, char separator, int index);
void source(float);
void parseIncomingMessage(const String &msg, String &command, std::vector<String> &values);
void handleAck(String &topic, float &currentValue, const float &receivedValue);
void handleHeartbeat(String &topic, std::vector<String> &values);
void handleInit(String &topic, std::vector<String> &values);
void handleTime(String &topic, String &value1);
void handleIRCode(unsigned long code);
void sendCommandToClient(const String& command, const String &value);
const char* getTopic(const String &topic);
double volumeControl(double position);
