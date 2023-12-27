//#include "INCLUDE.h"
//#include <WiFiClient.h>
//#include <PubSubClient.h>
//#include <mqtt.h>
//#include <multicast.h>
//#include <BluetoothA2DPSink.h>
#include "Arduino.h"
#include "AudioTools.h"
#include "AudioLibs/AudioA2DP.h"
#include "Communication/UDPStream.h"

#define SSID "ATT SUX"
#define PASSWORD "peanutapple42"

using namespace audio_tools;

//WiFiClient espClient;
//PubSubClient client(espClient);
//const char* mqttServer = "192.168.1.81";
//const int mqttPort = 1883;
//uint16_t port = 1234;


//BluetoothA2DPSink a2dp_sink;
//QueueHandle_t sendQueue;
//AudioBuffer buffer1 = AudioBuffer(44100, 16, 2, 70);
//AudioBuffer buffer2 = AudioBuffer(44100, 16, 2, 70);
//AudioBuffer* fillBuffer = &buffer1;
//AudioBuffer* sendBuffer = &buffer2;

AudioInfo info(44100, 2, 16);
A2DPStream in;
UDPStream udp(SSID, PASSWORD);
Throttle throttle(udp);
IPAddress udpAddress(192, 168, 1, 81);
const int udpPort = 7000;
StreamCopy copier(throttle, in);  // copies sound into i2s

void setup() {
    Serial.begin(115200);
    AudioLogger::instance().begin(Serial, AudioLogger::Info);
    //wifiSetup();
    //espClient.setNoDelay(true);
    udp.begin(udpAddress, udpPort);
    //client.setServer(IPAddress(192, 168, 1, 81), mqttPort);
    //client.setCallback(mqttCallback);
    //reconnect();
    //irSetup();
//    a2dp_sink.set_volume(80);
//    a2dp_sink.set_avrc_connection_state_callback(avrc_connection_state_callback);
//    a2dp_sink.set_stream_reader(callbackToReceiveData, false);
//    a2dp_sink.start("MyMusic", false);
//    sendQueue = xQueueCreate(10, sizeof(std::shared_ptr<AudioBuffer>));
//    xTaskCreatePinnedToCore(&AudioBuffer::sendBufferTcpTask, "SendTask", 20000, nullptr, 1, nullptr, 1);

//    start the bluetooth audio receiver
    Serial.println("starting A2DP...");
    auto cfg = in.defaultConfig(RX_MODE);
    cfg.name = "MyReceiver";
    in.begin(cfg);

    // Define udp address and port
    udp.begin(udpAddress, udpPort);

    // Define Throttle
    auto cfgThrottle = throttle.defaultConfig();
    cfgThrottle.copyFrom(info);
    //cfg.correction_ms = 0;
    throttle.begin(cfgThrottle);
    Serial.println("started...");
}

void loop() {
    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }
//    if (!client.connected()) {
//        reconnect();
//    }
//    client.loop();
    copier.copy();
}









