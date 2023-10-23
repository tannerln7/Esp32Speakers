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
    WiFi.mode(WIFI_STA);
    WiFi.begin("LivingRoom", "!Password!");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
}


void webServerSetup() {
    server.addHandler(&webserv);
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });
    server.begin();
}

