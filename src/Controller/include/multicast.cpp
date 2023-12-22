//
// Created by Tanner on 12/19/2023.
//

#include "multicast.h"
#include <array>


extern BluetoothA2DPSink a2dp_sink;
AudioBuffer buffer1 = AudioBuffer(44100, 16, 2, 20);
AudioBuffer buffer2 = AudioBuffer(44100, 16, 2, 20);
AudioBuffer* fillBuffer = &buffer1;
AudioBuffer* sendBuffer = &buffer2;

bool establishTCPConnection() {
    int i = 0;
    while (!espClient.connected() && WiFi.status() == WL_CONNECTED && i < 5) {
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
    if (espClient.connected()){
        Serial.println("Connected to TCP server.");
        return true;
    }
    Serial.println("Failed to connect to TCP server.");
    return false;
}

void callbackToReceiveData(const uint8_t *data, const uint32_t length) {
    // Lock the fill buffer mutex
    fillBuffer->lockBuffer();
    if (fillBuffer->appendData(data, length) == -1) {
        // Swap buffers
        std::swap(fillBuffer, sendBuffer);
        // Reset the fillBuffer index
        fillBuffer->ResetFillIndex();
        // Unlock the fill buffer mutex
        fillBuffer->unlockBuffer();
        // Append the data to the new fillBuffer
        fillBuffer->appendData(data, length);
        // Create the sendBufferTcp task and pass sendBuffer as a parameter
        xTaskCreate(sendBufferTcp, "sendBufferTask", 1024, nullptr, 1, nullptr);
    }else{
        fillBuffer->appendData(data, length);
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

