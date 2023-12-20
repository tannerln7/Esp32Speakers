//
// Created by Tanner on 12/19/2023.
//

#include "multicast.h"

#define BUFFER_SIZE 1024  // Define an appropriate buffer size

uint8_t audioBuffer1[BUFFER_SIZE];
uint8_t audioBuffer2[BUFFER_SIZE];
uint8_t* currentBuffer = audioBuffer1;
uint8_t* sendBuffer = audioBuffer2;
uint32_t bufferIndex = 0;
bool useBuffer1 = true;
WiFiClient tcpClient;

extern BluetoothA2DPSink a2dp_sink;
extern QueueHandle_t audioQueue;

void establishTCPConnection() {
    while (!tcpClient.connect("192.168.1.81", 1234)) {
        Serial.println("Attempting to connect to TCP server...");
        delay(5000);  // Wait for 5 seconds before retrying
    }
    Serial.println("Connected to TCP server");
}

void sendBufferTask(void *pvParameters) {
    auto* bufferToSend = (uint8_t*)pvParameters;
    if (tcpClient.connected()) {
        Serial.println("Sending buffer over TCP");
        tcpClient.write(bufferToSend, BUFFER_SIZE);
        Serial.println("Sent TCP packet");
    } else {
        Serial.println("TCP connection lost. Reconnecting...");
        establishTCPConnection();  // Attempt to re-establish the connection
    }
    // Signal that the buffer has been sent
    if (bufferToSend == audioBuffer1) {
        useBuffer1 = true;  // Buffer 1 can be reused
    } else {
        useBuffer1 = false; // Buffer 2 can be reused
    }

    vTaskDelete(nullptr); // Delete this task
}

void audioProcessingTask(void *pvParameters) {
    AudioData audioPacket{};

    while (true) {
        if (xQueueReceive(audioQueue, &audioPacket, portMAX_DELAY)) {
            // Determine which buffer to use
            currentBuffer = useBuffer1 ? audioBuffer1 : audioBuffer2;

            // Copy data to the current buffer
            uint32_t bytesToCopy = min(audioPacket.length, BUFFER_SIZE - bufferIndex);
            memcpy(currentBuffer + bufferIndex, audioPacket.data, bytesToCopy);
            bufferIndex += bytesToCopy;

            // Check if the current buffer is full
            if (bufferIndex >= BUFFER_SIZE) {
                // Swap buffers
                sendBuffer = currentBuffer;
                useBuffer1 = !useBuffer1;
                bufferIndex = 0;

                // Create a task to send the full buffer
                xTaskCreate(sendBufferTask, "SendBufferTask", 2048, sendBuffer, 1, nullptr);
            }

            // Free the dynamically allocated memory
            free(audioPacket.data);
        }
    }
}

void callbackToReceiveData(const uint8_t *data, uint32_t length) {
    // Dynamically allocate memory to store the PCM data
    auto* buffer = (uint8_t*)malloc(length);
    if (buffer == nullptr) {
        // Handle memory allocation failure
        return;
    }

    // Copy the PCM data to the newly allocated buffer
    memcpy(buffer, data, length);

    // Create an AudioData structure and enqueue it
    AudioData audioPacket{};
    audioPacket.data = buffer;
    audioPacket.length = length;
    // Send the audio data to the queue
    if (xQueueSend(audioQueue, &audioPacket, portMAX_DELAY) != pdPASS) {
        // Handle failed to send data to queue
        free(buffer);  // Don't forget to free the memory if sending to queue fails
    }
}

void avrc_connection_state_callback(bool connected) {
    if(connected) {
        if (!tcpClient.connected()) {
            Serial.println("Lost TCP connection. Attempting to reconnect...");
            establishTCPConnection();
        }
    }else{
        tcpClient.stop();
    }
}

