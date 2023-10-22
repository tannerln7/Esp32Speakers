//
// Created by Tanner on 10/22/2023.
//

#ifndef ESP32SPEAKERS_LOOPS_H
#define ESP32SPEAKERS_LOOPS_H


#include <INCLUDE.h>

void ackCheck();
void ackReset();
void initDebug();
void wifiSetup();
void bluetoothSetup();
void irSetup();
void webSocketSetup();
void webServerSetup();
void bufferSetup();


#endif //ESP32SPEAKERS_LOOPS_H
