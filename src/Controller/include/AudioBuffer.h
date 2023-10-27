#include <WString.h>
#include "esp_heap_caps.h"
#include "AsyncWebSocket.h"
#include "BluetoothA2DPSink.h"

#pragma once

const uint32_t delay_time = 2000;

void handleBufferOverflow();
void processFullBuffer();
void read_data_stream(const uint8_t *data, uint32_t length);
extern BluetoothA2DPSink a2dp_sink;
extern AsyncWebServer server;
extern AsyncWebSocket webserv;

struct AudioPacket {
    uint8_t data[4096];
    size_t size;
    bool isValid;
    uint timestamp;
    __attribute__((unused)) uint playAtTimestamp;
    AudioPacket() : data{0}, size(0), timestamp(0), isValid(false), playAtTimestamp(0) {}
};

class AudioBuffer {
public:
    AudioBuffer() = default;

    explicit AudioBuffer(size_t numPackets) :
            capacity(numPackets * 4096),  // Change here
            head(0), tail(0), length(0), buffer(nullptr) {

        buffer = (uint8_t*) heap_caps_malloc(capacity, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!buffer) {
            // Handle memory allocation failure
            ESP_LOGE("AudioBuffer", "Failed to allocate memory!");
        }
        buffer_mutex = xSemaphoreCreateMutex();
    }

    ~AudioBuffer() {
        if (buffer) {
            free(buffer);
        }
        vSemaphoreDelete(buffer_mutex);
    }

    bool addData(const uint8_t* data, size_t size) {
        if (xSemaphoreTake(buffer_mutex, portMAX_DELAY) != pdTRUE) {
            return false;
        }

        if (length + size > capacity) {
            xSemaphoreGive(buffer_mutex);
            return false;
        }

        for (size_t i = 0; i < size; ++i) {
            buffer[head] = data[i];
            head = (head + 1) % capacity;
        }
        length += size;

        xSemaphoreGive(buffer_mutex);
        return true;
    }

    bool isFull() const {
        return length >= capacity;
    }

    void clear() {
        head = tail = length = 0;
    }

    AudioPacket packetize() {
        if (xSemaphoreTake(buffer_mutex, portMAX_DELAY) != pdTRUE) {
            return AudioPacket{};
        }
        AudioPacket packet{};
        if (isFull()) {
            for (size_t i = 0; i < capacity; ++i) {
                packet.data[i] = buffer[(tail + i) % capacity];
            }
            packet.size = capacity;
            packet.timestamp = millis();
            packet.playAtTimestamp = packet.timestamp + delay_time;
            packet.isValid = true;

            clear();
        }
        xSemaphoreGive(buffer_mutex);
        return packet;
    }

private:
    uint8_t* buffer;
    size_t capacity;
    size_t head, tail, length;
    SemaphoreHandle_t buffer_mutex;
};

[[noreturn]] void BluetoothTask(void* pvParameters) {
    esp_task_wdt_delete(nullptr);
    a2dp_sink.start("MyMusic");
    a2dp_sink.set_raw_stream_reader(read_data_stream);

    // Initialize your audio buffer here...
    AudioBuffer audioBuffer(10); // just an example size

    while (true) {
        if (audioBuffer.isFull()) {
            processFullBuffer();  // Your function to process the buffer
        }

        // Some other logic here...

        // Yielding the CPU for a bit
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

extern AudioBuffer audioBuffer;

void handleBufferOverflow() {
    // This is just a placeholder. You could reset the buffer, log the event, etc.
    Serial.println("Warning: Buffer overflow detected!");
}

// Updated processFullBuffer function to handle double buffering
void processFullBuffer(){
    AudioPacket packet = audioBuffer.packetize();
    if (packet.isValid) {
        // Send the binary data directly via WebSocket
        webserv.binaryAll(packet.data, packet.size);
        Serial.println("Buffer processed and sent as binary WebSocket frame.");
    }
    audioBuffer.clear();  // Clear the buffer for the next data
}

void read_data_stream(const uint8_t *data, uint32_t length){
    // Add incoming audio data to the buffer
    if (!audioBuffer.addData(data, length)) {
        handleBufferOverflow();
        return;
    }

    // Check if the buffer is full and ready for packetization
    if (audioBuffer.isFull()) {
        processFullBuffer();
    }
}

void bluetoothSetup(){
    xTaskCreatePinnedToCore(
            BluetoothTask,      /* Task function */
            "BluetoothTask",    /* Task name */
            2000,               /* Stack size */
            nullptr,                /* Parameters */
            1,                   /* Priority */
            nullptr,                /* Task handle (if you want to reference it later, otherwise NULL) */
            1                    /* Core you want to run the task on, 0 or 1. 1 for the second core */
    );
}