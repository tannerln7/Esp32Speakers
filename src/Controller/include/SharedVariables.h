//
// Created by Tanner on 10/22/2023.
//
#pragma once

#include <Arduino.h>
#include "AsyncWebSocket.h"
#include <BluetoothA2DPSink.h>
#include <AudioBuffer.h>


#ifndef ESP32SPEAKERS_SHAREDVARIABLES_H
#define ESP32SPEAKERS_SHAREDVARIABLES_H

#endif //ESP32SPEAKERS_SHAREDVARIABLES_H

extern AsyncWebSocket webserv;
extern BluetoothA2DPSink a2dp_sink;
extern AudioBuffer *audioBuffer;

//start-up DSP settings
extern float currentLinearVolume;
extern float currentVolumeDb;
extern float currentLinearSubVolume;
extern float currentSubVolumeDb;
extern int muteState;
extern int currentSource;

//constants

const unsigned long debounceDelay = 200;
const unsigned long initDebounce = 3000;

//websocket variables
extern String lastMessageLeft;
extern String lastMessageRight;
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

extern bool leftInit;
extern bool rightInit;
extern unsigned long lastLeftInitTime;
extern unsigned long lastRightInitTime;

bool timeSync;

const uint32_t delay_time = 2000;