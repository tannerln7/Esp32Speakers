#include "INCLUDE.h"
#include "Loops.h"
#include "AudioBuffer.h"
#include <Arduino.h>
#include <IRTask.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

BluetoothA2DPSink a2dp_sink;
AudioBuffer audioBuffer;

WebsocketsServer wsServer;
WebsocketsClient wsClient;

void setup() {
    Serial.begin(115200);
    //audioBuffer = AudioBuffer(10);
    //ws_mutex = xSemaphoreCreateMutex();
    wifiSetup();
    webSocketSetup();
    //bluetoothSetup();
    irSetup();
    setupAck();
}

void loop() {
//Serial read for restart
    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }
    wsServer.poll();
    wsClient.poll();
    //ackCheck();
    //ackReset();
    initDebug();
    webSocketLoop();
    //currentHeap();
}







