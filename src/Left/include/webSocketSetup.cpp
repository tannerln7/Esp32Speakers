//
// Created by Tanner on 10/30/2023.
//

#include "webSocketSetup.h"

const char* ssid = "ATT SUX";
const char* password = "peanutapple42";

using namespace websockets;

void onMessageCallback(const WebsocketsMessage& message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    handleCallback(message.data(), message.length());
}

void onEventsCallback(WebsocketsEvent event, const String& data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}

void webSocketSetup(){
    //WiFi
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    WebsocketsClient client;

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);

    //WebSocket
    client.connect("ws://192.168.1.198/audio");
    // Send a message
    client.send("Hi Server!");
    // Send a ping
    client.ping();
}