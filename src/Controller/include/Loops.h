//
// Created by Tanner on 10/22/2023.
//

#ifndef ESP32SPEAKERS_LOOPS_H
#define ESP32SPEAKERS_LOOPS_H
#endif //ESP32SPEAKERS_LOOPS_H

#include <SharedVariables.h>
#include "BluetoothA2DPSink.h"
#include "AsyncWebSocket.h"


void setupAck();

[[noreturn]] void ackTask(void *pvParameters);
void initDebug();
void wifiSetup();
void bluetoothSetup();
void webServerSetup();


