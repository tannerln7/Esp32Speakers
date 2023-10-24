//
// Created by Tanner on 10/22/2023.
//


#include <Loops.h>

unsigned long lastLeftInitTime = 0;
unsigned long lastRightInitTime = 0;
const unsigned long initDebounce = 15000;

void ackCheck(){
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
}

void ackReset(){
        if (rightAck) {
            retryCountRight = 0;
        }

        if (leftAck) {
            retryCountLeft = 0;
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
    Serial.begin(115200);
    delay(10);
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
    server.addHandler(&webserv);
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });
    server.begin();
    Serial.println("Web Server Started");
}

