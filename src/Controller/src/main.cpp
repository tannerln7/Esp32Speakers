#include "INCLUDE.h"
#include <WiFiClient.h>
#include <PubSubClient.h>
#include <mqtt.h>
#include <multicast.h>
#include <BluetoothA2DPSink.h>


WiFiClient espClient;
PubSubClient client(espClient);
const char* mqttServer = "mainsail.local";
const int mqttPort = 1883;

QueueHandle_t audioQueue;
BluetoothA2DPSink a2dp_sink;
void setup() {
    Serial.begin(115200);
    wifiSetup();
    establishTCPConnection();
    client.setServer(mqttServer, mqttPort);
    client.setCallback(mqttCallback);
    irSetup();
    reconnect();
    // Initialize the queue
    audioQueue = xQueueCreate(10, sizeof(AudioData));  // Adjust the size as needed

    // Check if queue creation was successful
    if (audioQueue == nullptr) {
        // Handle queue creation failure
        Serial.println("Failed to create audio queue");
        return;
    }
    a2dp_sink.set_volume(80);
    a2dp_sink.set_avrc_connection_state_callback(avrc_connection_state_callback);

    // Start the audio processing task
    xTaskCreate(audioProcessingTask, "AudioProcessingTask", 10000, nullptr, 10, nullptr);
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
    initDebug();
    client.loop();
    delay(100);
}







