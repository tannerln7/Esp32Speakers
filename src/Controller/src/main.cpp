#include "INCLUDE.h"
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <mqtt.h>
#include <multicast.h>
#include <BluetoothA2DPSink.h>


WiFiClient espClient;
PubSubClient client(espClient);
const char* mqttServer = "192.168.1.81";
const int mqttPort = 1883;
BluetoothA2DPSink a2dp_sink;

void setup() {
    Serial.begin(115200);
    wifiSetup();
    client.setServer(IPAddress(192, 168, 1, 81), mqttPort);
    client.setCallback(mqttCallback);
    reconnect();
    establishTCPConnection();
    irSetup();
    a2dp_sink.set_volume(80);
    a2dp_sink.set_avrc_connection_state_callback(avrc_connection_state_callback);
    a2dp_sink.set_stream_reader(callbackToReceiveData, false);
    a2dp_sink.start("MyMusic", false);
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
    client.loop();
    delay(10);
}









