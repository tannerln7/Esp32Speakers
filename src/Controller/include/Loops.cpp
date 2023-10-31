//
// Created by Tanner on 10/22/2023.
//


#include <Loops.h>
#include <ArduinoWebsockets.h>

using namespace websockets;
unsigned long lastInit = 0;
unsigned long lastSendTime = 0;
const unsigned long initDebounce = 15000;
unsigned long retryCount = 0;

extern WebsocketsClient wsClient;
extern WebsocketsServer wsServer;
String lastMessage;
bool isInit;
bool ack;
// Initialize the mutex semaphore


[[noreturn]] void ackTask(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // Frequency in ms, adjust as needed

    // Initialize the last wake time variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    while (true) {
        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Perform ackCheck
        if (!ack && millis() - lastSendTime > 500 && retryCount < 5) {
            wsClient.send(lastMessage);
            lastSendTime = millis();
            retryCount++;
        }

        // Perform ackReset
        if (ack) {
            retryCount = 0;
        }
    }
}


void setupAck() {
    // Create the ackTask
    BaseType_t taskCreated = xTaskCreate(
            ackTask,          // Task function
            "AckTask",        // A name for the task for debugging
            2000,             // Stack size (in words, not bytes)
            nullptr,             // Parameters to pass to the task function
            1,                // Priority (0 is lowest, higher numbers are higher priority)
            nullptr              // Task handle (not used in this example)
    );

    if (taskCreated != pdPASS) {
        Serial.println("Failed to create ackTask");
        // Take corrective action here
    }else {
        Serial.println("ackTask created successfully");
    }
}

void initDebug(){
        if (!isInit && millis() - lastInit > initDebounce) {
            Serial.println("Waiting for Left Init...");
            lastInit = millis();
        }
}

void wifiSetup() {
    const char* ssid = "ATT SUX";
    const char* password = "peanutapple42";
    Serial.println("\nConnecting to Wi-Fi...");

    WiFi.scanNetworks();

    WiFi.mode(WIFI_STA); // Set mode to station to connect to the router
    WiFi.begin(ssid, password); // Connect to Wi-Fi

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected to Wi-Fi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Wifi Setup Complete");
}

void webSocketLoop() {
    wsClient.poll();
    wsServer.poll();
    auto client = wsServer.accept();
    if(client.available()) {
        auto msg = client.readBlocking();

        // log
        Serial.print("Got Message: ");
        Serial.println(msg.data());
    }
}