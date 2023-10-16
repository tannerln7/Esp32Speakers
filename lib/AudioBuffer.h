#include <WString.h>
#include "ArduinoJson.h"

const uint32_t delay_time = 2000;  // 2000 milliseconds = 2 seconds

struct AudioPacket {
    uint8_t data[4096];
    size_t size;
    bool isValid;
    uint timestamp;
    uint playAtTimestamp;
    // Default constructor
    AudioPacket() : data{0}, size(0), timestamp(0), isValid(false), playAtTimestamp(0) {}
};

class AudioBuffer {
public:
    explicit AudioBuffer(size_t size): buffer(new uint8_t[size]), capacity(size), length(0), head(0), tail(0) {
        buffer_mutex = xSemaphoreCreateMutex();
    }

    ~AudioBuffer() {
        delete[] buffer;
        vSemaphoreDelete(buffer_mutex);
    }

    bool addData(const uint8_t* data, size_t size) {
        xSemaphoreTake(buffer_mutex, portMAX_DELAY);
        if (length + size > capacity) {
            xSemaphoreGive(buffer_mutex);
            return false;  // Buffer overflow
        }

        for (size_t i = 0; i < size; ++i) {
            buffer[head] = data[i];
            head = (head + 1) % capacity;
        }
        length += size;
        xSemaphoreGive(buffer_mutex);
        return true;
    }

    bool isFull() {
        xSemaphoreTake(buffer_mutex, portMAX_DELAY);
        bool full = length >= capacity;
        xSemaphoreGive(buffer_mutex);
        return full;
    }

    void clear() {
        xSemaphoreTake(buffer_mutex, portMAX_DELAY);
        head = tail = length = 0;
        xSemaphoreGive(buffer_mutex);
    }

    AudioPacket packetize() {
        xSemaphoreTake(buffer_mutex, portMAX_DELAY);
        if (isFull()) {
            AudioPacket packet{};
            for (size_t i = 0; i < capacity; ++i) {
                packet.data[i] = buffer[(tail + i) % capacity];
            }
            packet.size = capacity;
            packet.timestamp = millis();
            packet.playAtTimestamp = packet.timestamp + delay_time;  // set the future timestamp
            packet.isValid = true;
            xSemaphoreGive(buffer_mutex);
            return packet;
        } else {
            xSemaphoreGive(buffer_mutex);
            AudioPacket emptyPacket{};
            emptyPacket.isValid = false;
            return emptyPacket;
        }
    }


private:
    uint8_t* buffer;
    size_t capacity;
    size_t length;
    size_t head;
    size_t tail;
    SemaphoreHandle_t buffer_mutex;
};
