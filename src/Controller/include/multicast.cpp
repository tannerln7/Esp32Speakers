//
// Created by Tanner on 12/19/2023.
//

#include "multicast.h"
#include <array>


extern BluetoothA2DPSink a2dp_sink;
extern AudioBuffer buffer1;
extern AudioBuffer buffer2;
extern AudioBuffer* fillBuffer;
extern AudioBuffer* sendBuffer;


bool establishTCPConnection() {
    int i = 0;
    while (!espClient.connected() && i < 5) {
        Serial.println("Attempting to connect to TCP server...");
        if (espClient.connect(IPAddress(192, 168, 1, 81), 1234)) {
            Serial.println("Connected to TCP server.");
            return true;
        }
        else {
            Serial.println("Failed to connect to TCP server. Retrying in 1 second...");
            delay(1000);
            i++;
        }
    }
    Serial.println("Failed to connect to TCP server.");
    return false;
}

void callbackToReceiveData(const uint8_t *data, const uint32_t length) {
    // check if the buffer is full
    if (fillBuffer->appendData(data, length) == -1) {
        // Swap buffers
        std::swap(fillBuffer, sendBuffer);
        Serial.println("Buffers swapped");

        // Send the buffer to the send queue
        if (!xQueueSend(sendQueue, &(sendBuffer), (TickType_t) 10)) {
            Serial.println("Failed to post the message, even after 10 ticks");
        } else {
            Serial.println("Full buffer sent to queue.");
            // Reset the fill buffer upon successful sending
            fillBuffer->resetBuffer();
            Serial.println("Buffer reset");

            // Append the data to the new fillBuffer
            if (fillBuffer->appendData(data, length) == -1) {
                Serial.println("Failed to append data to buffers.");
            }else{
                Serial.println("Data appended to new buffer.");
            }
        }
    }
}




    void avrc_connection_state_callback(bool connected) {
    if (connected) {
        if (!espClient.connected()) {
            Serial.println("Bluetooth device connected, connecting to TCP server...");
            establishTCPConnection();
        }
    }
}

