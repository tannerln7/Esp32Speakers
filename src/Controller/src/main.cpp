#include "INCLUDE.h"
#include "Loops.h"
#include "AudioBuffer.h"
#include <Arduino.h>
#include <IRTask.h>

AsyncWebSocket webserv("/audio");
AsyncWebServer server(80);
BluetoothA2DPSink a2dp_sink;
AudioBuffer audioBuffer;
SemaphoreHandle_t ws_mutex;


void setup() {
    Serial.begin(115200);
    //audioBuffer = AudioBuffer(10);
    //ws_mutex = xSemaphoreCreateMutex();
    wifiSetup();
    webServerSetup();
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
    //ackCheck();
    //ackReset();
    //initDebug();
    //currentHeap();
}







