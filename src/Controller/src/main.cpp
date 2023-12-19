#include "INCLUDE.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <mqtt.h>
#include <multicast.h>


WiFiClient espClient;
PubSubClient client(espClient);


void setup() {
    Serial.begin(115200);
    wifiSetup();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(mqttCallback);
    irSetup();
    reconnect();
    audioQueue = xQueueCreate(10, sizeof(uint8_t*));
    xTaskCreate(audioProcessingTask, "AudioProcessingTask", 10000, NULL, 1, NULL);
}

void loop() {
    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }
    if (!client.connected()) {
        reconnect();
    }
    initDebug();
    client.loop();
    delay(10);
}







