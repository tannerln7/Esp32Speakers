//
// Created by Tanner on 12/19/2023.
//

// Updated Code Snippet

#include "multicast.h"

const int sampleRate = 44100;  // Sample rate in Hz
const int channels = 2;        // Stereo
const int chunkTimeMs = 50;   // Desired chunk size in milliseconds
const int bytesPerSample = 4;  // 16-bit stereo audio

extern WiFiClient espClient;
extern BluetoothA2DPSink a2dp_sink;

void establishTCPConnection() {
    while (!espClient.connect(IPAddress(192, 168, 1, 81), 1234)) {
        Serial.println("Attempting to connect to TCP server...");
        delay(1000);  // Wait for 5 seconds before retrying
    }
    Serial.println("Connected to TCP server");
}

void callbackToReceiveData(const uint8_t *data, uint32_t length) {
    // Calculate the chunk size based on the desired ms
    const uint32_t chunkSize = (sampleRate / 1000) * chunkTimeMs * bytesPerSample;
    Serial.println("Received data - size: " + (String) length + " bytes");

    // Process the entire length of received data
    for (uint32_t i = 0; i < length; i += chunkSize) {
        // Calculate the size of the current segment
        uint32_t segmentSize = min(chunkSize, length - i);

        // Check if TCP client is connected before sending
        if (!espClient.connected()) {
            Serial.println("TCP client not connected.");
            establishTCPConnection();
            return;
        }

        // Send the segment directly to the TCP server
        espClient.write(data + i, segmentSize);
        Serial.println("Sent segment of data - size: " + (String) segmentSize + " bytes");
    }
}


void avrc_connection_state_callback(bool connected) {
    if (connected) {
        if (!espClient.connected()) {
            Serial.println("Lost TCP connection. Attempting to reconnect...");
            establishTCPConnection();
        }
    }
}

