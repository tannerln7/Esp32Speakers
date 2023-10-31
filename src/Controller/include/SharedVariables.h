//
// Created by Tanner on 10/22/2023.
//
#pragma once

#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#ifndef ESP32SPEAKERS_SHAREDVARIABLES_H
#define ESP32SPEAKERS_SHAREDVARIABLES_H

#endif //ESP32SPEAKERS_SHAREDVARIABLES_H

//start-up DSP settings
extern float currentLinearVolume;
extern float currentVolumeDb;
extern float currentLinearSubVolume;
extern float currentSubVolumeDb;
extern int muteState;
extern int currentSource;
extern String lastMessage;
extern bool ack;
extern bool isInit;
extern unsigned long lastSendTime;
extern unsigned long retryCount;


//constants


