#include "INCLUDE.h"
#include <WiFi.h>
#include <PubSubClient.h>
//#include <BluetoothA2DPSink.h>
//#include <WebSocketsServer.h>
#include <AsyncTCP.h>
//#include <websocket.h>
#include <WiFiMulti.h>

const char* ssid = "ssid";
const char* password = "password";
const char* mqttServer = "mainsail.local";
const int mqttPort = 1883;
const char* mqttUser = "username";
const char* mqttPassword = "password";
const char* leftTopic = "home/livingroom/left/command";
const char* leftAckTopic = "home/livingroom/left/ack";
const char* rightTopic = "home/livingroom/right/command";
const char* rightAckTopic = "home/livingroom/right/ack";

WiFiMulti WiFiMulti;
//WebSocketsServer webSocket = WebSocketsServer(81);
//String wsUrl;
WiFiClient espClient;
PubSubClient client(espClient);
//BluetoothA2DPSink a2dpSink;

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
    Serial.setDebugOutput(true);
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("[SETUP] BOOT WAIT %d...\n", t);
        Serial.flush();
        delay(1000);
    }
    WiFiMulti.addAP(ssid, password);
    while(WiFiMulti.run() != WL_CONNECTED) {
        delay(100);
    }
//    webSocket.begin();
//    webSocket.onEvent(webSocketEvent);

    //wifiSetup();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);
//    a2dpSink.start("ESP32_AudioSink");
//    a2dpSink.set_stream_reader(audioDataCallback, false);
//    a2dpSink.set_on_data_received(data_received_callback);
    irSetup();
    reconnect();
//    wsUrl = "ws://";
//    wsUrl += WiFi.localIP().toString();
//    wsUrl += ":81/ws"; // Assuming WebSocket server is on port 81 and path "/ws"
    Serial.println("Wifi, MQTT, Bluetooth, and IR setup complete");
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
//    webSocket.loop();
    client.loop();
    delay(10);
}








