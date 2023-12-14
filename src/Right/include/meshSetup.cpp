//
// Created by Tanner on 10/30/2023.
//

#include <meshSetup.h>
#include <WiFi.h>

void wifiSetup() {
    delay(10);
    const char* ssid = "ATT SUX";
    const char* password = "peanutapple42";
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

