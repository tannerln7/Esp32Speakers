//
// Created by Tanner on 10/22/2023.
//


#include <Loops.h>

unsigned long lastInit = 0;
const unsigned long initDebounce = 15000;
bool leftInit;
bool rightInit;

void initDebug() {
    if (!leftInit && millis() - lastInit > initDebounce) {
        Serial.println("Waiting for Left Init...");
        lastInit = millis();
    }
    if (!rightInit && millis() - lastInit > initDebounce) {
        Serial.println("Waiting for Right Init...");
        lastInit = millis();
    }
}

void wifiSetup() {
    const char *ssid = "ATT SUX";
    const char *password = "peanutapple42";
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}