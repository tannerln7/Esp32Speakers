#include "INCLUDE.h"
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "ATT SUX";
const char* password = "peanutapple42";
const char* mqttServer = "mainsail.local";
const int mqttPort = 1883;
const char* mqttUser = "tannerln7";
const char* mqttPassword = "Muspotaebo1324";
const char* leftTopic = "home/livingroom/left/command";
const char* leftAckTopic = "home/livingroom/left/ack";
const char* rightTopic = "home/livingroom/right/command";
const char* rightAckTopic = "home/livingroom/right/ack";
WiFiClient espClient;
PubSubClient client(espClient);

//BluetoothA2DPSink a2dp_sink;
//AudioBuffer audioBufferr;

void callback(const char* topic, byte* payload, unsigned int length) {
    String message;
    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }
    Serial.println(message);
    Serial.println(topic);
    handleCallback(topic, message);
}

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

void setup() {
    Serial.begin(115200);
    //audioBuffer = AudioBuffer(10);
    //ws_mutex = xSemaphoreCreateMutex();
    //bluetoothSetup();
    wifiSetup();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
    irSetup();
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
    initDebug();
    client.loop();
    delay(10);
}







