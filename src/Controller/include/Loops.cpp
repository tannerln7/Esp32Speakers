//
// Created by Tanner on 10/22/2023.
//


#include <Loops.h>

unsigned long lastLeftInitTime = 0;
unsigned long lastRightInitTime = 0;
const unsigned long initDebounce = 15000;
// Initialize the mutex semaphore
extern AsyncWebServer server;
extern AsyncWebSocket webserv;

[[noreturn]] void ackTask(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // Frequency in ms, adjust as needed

    // Initialize the last wake time variable with the current time.
    xLastWakeTime = xTaskGetTickCount();

    while (true) {
        // Wait for the next cycle.
        vTaskDelayUntil(&xLastWakeTime, xFrequency);

        // Perform ackCheck
        if (!leftAck && millis() - lastSendTimeLeft > 500 && retryCountLeft < 5) {
            webserv.text(leftId, lastMessageLeft);
            lastSendTimeLeft = millis();
            retryCountLeft++;
        }

        if (!rightAck && millis() - lastSendTimeRight > 500 && retryCountRight < 5) {
            webserv.text(rightId, lastMessageRight);
            lastSendTimeRight = millis();
            retryCountRight++;
        }

        // Perform ackReset
        if (rightAck) {
            retryCountRight = 0;
        }

        if (leftAck) {
            retryCountLeft = 0;
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
        if (!leftInit && millis() - lastLeftInitTime > initDebounce) {
            Serial.println("Waiting for Left Init...");
            lastLeftInitTime = millis();
        }
        if (!rightInit && millis() - lastRightInitTime > initDebounce) {
            Serial.println("Waiting for Right Init...");
            lastRightInitTime = millis();
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


void webServerSetup() {
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });
    server.begin();
    Serial.println("Web Server Started");
}

