//
// Created by Tanner on 12/19/2023.
//

#ifndef SPEAKERS_MULTICAST_H
#define SPEAKERS_MULTICAST_H
#include <Arduino.h>
#include "BluetoothA2DPSink.h"
#include <WiFi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <WiFiClient.h>
#include <vector>
#include <mutex>

void callbackToReceiveData(const uint8_t *data,  uint32_t length);
bool establishTCPConnection();
void avrc_connection_state_callback(bool connected);
extern QueueHandle_t sendQueue;
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
            Serial.println("Buffer locked.");
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
            Serial.println("Buffer unlocked.");
            return true;
        }
    }

    void resetBuffer() {
        if (!lockBuffer()) {
            Serial.println("Failed to lock buffer.");
            return; // Return if the buffer is not locked
        }
        fillIndex = 0;
        std::fill(buffer.begin(), buffer.end(), 0);
        unlockBuffer();
    }

    int appendData(const uint8_t* data, const uint32_t length) {
        if (!lockBuffer()) {
            // Return -2 to indicate failed to lock the buffer
            Serial.println("Failed to lock buffer.");
            return -2;
        }
        if(fillIndex + length > bufferSize){
            Serial.println("Buffer full.");
            Serial.println("fillIndex: " + String(fillIndex) + " length: " + String(length) + " bufferSize: " + String(bufferSize));
            unlockBuffer();
            return -1; // Buffer overflow
        }
        std::copy(data, data + length, buffer.begin() + fillIndex);
        fillIndex += length;
        if (!unlockBuffer()) {
            return -3; // Failed to unlock the buffer
        }
        Serial.println("Data appended to buffer.");
        return 0; // Success
    }

    bool isFull(const uint32_t length) const {
        if (xSemaphoreTake(Buffer_mutex, portMAX_DELAY) != pdTRUE) {
            Serial.println("Failed to lock buffer with semaphore.");
            return true;
        }

        bool isFull = fillIndex + length > bufferSize;

        if (isFull) {
            Serial.println("Buffer will overflow.");
        }

        xSemaphoreGive(Buffer_mutex);

        return isFull;
    }

    [[noreturn]] static void sendBufferTcpTask(void *pvParameters) {
        while(true){
            std::shared_ptr<AudioBuffer> buffer;
            if(xQueueReceive(sendQueue, &(buffer), (TickType_t) portMAX_DELAY))
            {
                if(buffer){
                    if(!espClient.connected()){
                        if(!establishTCPConnection()){
                            continue;
                        }
                    }
                    if(!buffer->lockBuffer()){
                        continue;
                    }
                    Serial.println("TCP packet sent - size: ");
                    Serial.println(espClient.write(buffer->buffer.data(), buffer->fillIndex));
                    buffer->resetBuffer();
                    buffer->unlockBuffer();
                    vTaskDelay(5);
                }
            }
        }
    }

private:
    const int sampleRate;
    const int channels;
    const int bytesPerSample;
    int chunkTimeMs;
    int bufferSize;
    uint32_t fillIndex;
    std::vector<uint8_t> buffer;
    SemaphoreHandle_t Buffer_mutex;
};

#endif //SPEAKERS_MULTICAST_H
