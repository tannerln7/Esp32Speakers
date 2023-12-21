//
// Created by Tanner on 12/19/2023.
//

#ifndef SPEAKERS_MULTICAST_H
#define SPEAKERS_MULTICAST_H
#pragma once
#include "BluetoothA2DPSink.h"
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <WiFiClient.h>

#define MAX_QUEUE_ITEMS 10  // Define the maximum number of items in the queue

// Structure to hold audio data
struct AudioData {
    uint8_t* data;
    uint32_t length;
};

[[noreturn]] void audioProcessingTask(void *pvParameters);
void callbackToReceiveData(const uint8_t *data, uint32_t length);
void establishTCPConnection();
void avrc_connection_state_callback(bool connected);

[[noreturn]] void sendBufferTask(void *pvParameters);

#endif //SPEAKERS_MULTICAST_H
