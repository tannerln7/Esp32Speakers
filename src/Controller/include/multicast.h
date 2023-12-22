//
// Created by Tanner on 12/19/2023.
//

#ifndef SPEAKERS_MULTICAST_H
#define SPEAKERS_MULTICAST_H
#pragma once
#include <Arduino.h>
#include "BluetoothA2DPSink.h"
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <WiFiClient.h>
#include <vector>
#include <mutex>

void callbackToReceiveData(const uint8_t *data, uint32_t length);
bool establishTCPConnection();
void avrc_connection_state_callback(bool connected);

extern WiFiClient espClient;

class AudioBuffer {
public:
    explicit AudioBuffer(const int sampleRate, const int bitdepth, const int channels, const int time_ms) : sampleRate(sampleRate),
        bytesPerSample((bitdepth / 8) * channels),
        channels(channels),
        chunkTimeMs(time_ms),
        bufferSize((sampleRate / 1000) * chunkTimeMs * bytesPerSample),
        fillIndex(0),
        buffer(bufferSize) {
        Buffer_mutex = xSemaphoreCreateMutex();
        if(Buffer_mutex == nullptr){
            Serial.println("Failed to create mutex for buffer.");
        }
    }

    ~AudioBuffer() {
        vSemaphoreDelete(Buffer_mutex);
    }

    // Delete copy constructor and copy assignment operator
    //AudioBuffer(const AudioBuffer&) = delete;
    AudioBuffer& operator=(const AudioBuffer&) = delete;

public:
    bool lockBuffer() {
        // Check if the buffer is already locked
        if (xSemaphoreTake(Buffer_mutex, portMAX_DELAY) != pdTRUE){
            Serial.println("Failed to lock buffer with semaphore. Potential deadlock situation.");
            // Return false to indicate failure
            return false;
        }else {
            // Return true to indicate success
            return true;
        }
    }

    bool unlockBuffer() {
        // Check if the buffer can be unlocked
        if (xSemaphoreGive(Buffer_mutex) != pdTRUE){
            Serial.println("Failed to unlock buffer with semaphore. Potential deadlock situation.");
            // Return false to indicate failure
            return false;
        }else{
            // Return true to indicate success
            return true;
        }
    }

    int appendData(const uint8_t* data, const uint32_t length) {
        // Check if the buffer will overflow
        if(isFull(length)) {
            Serial.println("Buffer is overflowed.");
            return -1; // Return -1 to indicate buffer overflow
        }

        // Attempt to lock the buffer
        if (!lockBuffer()) {
            return -2; // Return -2 to indicate failed to lock the buffer
        }

        // Copy the data into the buffer
        std::copy(data, data + length, buffer.begin() + fillIndex);
        fillIndex += length;

        // Attempt to unlock the buffer
        if (!unlockBuffer()) {
            return -3; // Return -3 to indicate failed to unlock the buffer
        }

        return 0; // Return 0 to indicate success
    }

    bool isFull(const uint32_t length) const {
        // Check if the buffer will overflow
        if(fillIndex + length > GetBufferSize()){
            Serial.println("Buffer will overflow.");
            // Return true to indicate buffer overflow
            return true;
        }else{
            // Return false to indicate buffer will not overflow
            return false;}
    }
    int GetBufferSize() const { return bufferSize; }
    void ResetFillIndex() {
        fillIndex = 0;
    }
    void sendBufferTcp(void *pvParameters) {
        // Check if the TCP client is connected
        if(!espClient.connected()){
            establishTCPConnection();
        }
        // Lock the buffer mutex
        if (!lockBuffer()) {
            Serial.println("Buffer is locked by another process. Packet dropped.");
            return;
        } else {
            Serial.print("TCP packet sent - size: ");
            // Send the buffer over TCP
            Serial.println(espClient.write(buffer.data(), fillIndex));
            // Reset the fill index
            fillIndex = 0;
            // Unlock the send buffer mutex
            unlockBuffer();
            // Delete the task
            vTaskDelete(nullptr);
        }
    }

private:
    const int sampleRate;
    const int channels;
    const int bytesPerSample;
    int chunkTimeMs;
    int bufferSize;
    int fillIndex;
    std::vector<uint8_t> buffer;
    SemaphoreHandle_t Buffer_mutex;
};

#endif //SPEAKERS_MULTICAST_H
