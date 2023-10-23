//
// Created by Tanner on 10/22/2023.
//

#ifndef ESP32SPEAKERS_INCLUDE_H
#define ESP32SPEAKERS_INCLUDE_H

#endif //ESP32SPEAKERS_INCLUDE_H

#pragma once

#include <IRrecv.h>
#include <IRutils.h>
#include "BluetoothA2DPSink.h"
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include <map>
#include "AsyncWebSocket.h"
#include "Message.h"
#include "Handlers.h"
#include "SharedVariables.h"
#include <Debug.h>