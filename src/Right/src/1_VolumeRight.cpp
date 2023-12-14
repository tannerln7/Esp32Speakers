#include <Wire.h>
#include <SigmaDSP.h>
#include <dspSetup.h>
#include <Arduino.h>
#include <Loops.h>
#include <meshSetup.h>
#include <PubSubClient.h>
#include <WiFi.h>

const char* ssid = "ATT SUX";
const char* password = "peanutapple42";
const char* mqttServer = "mainsail.local";
const int mqttPort = 1883;
const char* mqttUser = "tannerln7";
const char* mqttPassword = "Muspotaebo1324";
const char* rightTopic = "home/livingroom/right/command";
const char* rightAckTopic = "home/livingroom/right/ack";

SigmaDSP dsp(Wire, DSP_I2C_ADDRESS, 96000.00f /*,12*/);
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    handleCallback(message);
}

void reconnect() {
    while (!client.connected()) {
        // Attempt to connect with username and password
        Serial.println("Attempting MQTT connection...");
        String clientId = "Right";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
            Serial.println("connected");
            client.subscribe(rightTopic);
            client.publish(rightAckTopic, "Right Online");
            Serial.println("Right Online");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    dspSetup();
    wifiSetup();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    reconnect();
}

void loop() {
    //Serial read for restart
    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }
    if (!client.connected()) {
        reconnect();
    }
    ackLoop();
    heartBeatLoop();
    client.loop();
    delay(10);
}