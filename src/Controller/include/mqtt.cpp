//
// Created by LANETAN on 12/19/2023.
//

#include "mqtt.h"

const char* mqttUser = "tannerln7";
const char* mqttPassword = "Muspotaebo1324";
const char* leftTopic = "home/livingroom/left/command";
const char* leftAckTopic = "home/livingroom/left/ack";
const char* rightTopic = "home/livingroom/right/command";
const char* rightAckTopic = "home/livingroom/right/ack";

extern void handleCallback(String topic, String &msg);

void reconnect() {
    while (!client.connected()) {
        // Attempt to connect with username and password
        Serial.println("Attempting MQTT connection...");
        String clientId = "Server";
        clientId += String(random(0xffff), HEX);
        if (client.connect(clientId.c_str(), mqttUser, mqttPassword)) {
            Serial.println("connected");
            client.subscribe(leftAckTopic);
            client.subscribe(rightAckTopic);
            client.publish(leftTopic, "Server Online");
            client.publish(rightTopic, "Server Online");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void mqttCallback(const char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    Serial.println(topic);
    handleCallback(topic, message);
}