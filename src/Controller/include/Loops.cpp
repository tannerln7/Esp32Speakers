//
// Created by Tanner on 10/22/2023.
//

#include "INCLUDE.h"
#include "SharedVariables.h"

void ackCheck(){
        if (!leftAck && millis() - lastSendTimeLeft > 500 && retryCountLeft < 5) {
            ws.text(leftId, lastMessageLeft);
            lastSendTimeLeft = millis();
            retryCountLeft++;
        }
        if (!rightAck && millis() - lastSendTimeRight > 500 && retryCountRight < 5) {
            ws.text(rightId, lastMessageRight);
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

void bluetoothSetup() {
    a2dp_sink.start("MyMusic");
    a2dp_sink.set_stream_reader(read_data_stream, false);
}

void irSetup() {
    xTaskCreatePinnedToCore(
            IRReceiverTask,      /* Task function */
            "IRReceiverTask",    /* Task name */
            2000,               /* Stack size */
            nullptr,                /* Parameters */
            1,                   /* Priority */
            nullptr,                /* Task handle (if you want to reference it later, otherwise NULL) */
            1                    /* Core you want to run the task on, 0 or 1. 1 for the second core */
    );
}

void webServerSetup() {
    server.addHandler(&ws);
    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404);
    });
    server.begin();
}

void webSocketSetup() {
    ws.onEvent(onWsEvent);
}

void bufferSetup() {
    if (esp_spiram_is_initialized()) {
        audioBuffer = new AudioBuffer(10);
        Serial.println("SPIRAM initialized");
    }else Serial.println("SPIRAM not initialized");
}
