//
// Created by Tanner on 10/22/2023.
//
#pragma once

#include <Arduino.h>
#include <AsyncWebSocket.h>

#ifndef ESP32SPEAKERS_SHAREDVARIABLES_H
#define ESP32SPEAKERS_SHAREDVARIABLES_H

#endif //ESP32SPEAKERS_SHAREDVARIABLES_H

extern AsyncWebServer server;
extern AsyncWebSocket webserv;

//start-up DSP settings
extern float currentLinearVolume;
extern float currentVolumeDb;
extern float currentLinearSubVolume;
extern float currentSubVolumeDb;
extern int muteState;
extern int currentSource;

//constants

extern const unsigned long debounceDelay;

extern unsigned long lastSendTimeLeft;
extern unsigned long lastSendTimeRight;
extern int retryCountLeft;
extern int retryCountRight;
extern uint32_t myID;
extern uint32_t leftId;
extern uint32_t rightId;
extern bool leftAck;
extern bool rightAck;
extern bool leftTime;
extern bool rightTime;
extern String lastMessageLeft;
extern String lastMessageRight;

extern bool leftInit;
extern bool rightInit;
