#include "INCLUDE.h"


AsyncWebServer server(WEBSOCKET_PORT);
AsyncWebSocket webserv("/audio");
AudioBuffer *audioBuffer = nullptr;
BluetoothA2DPSink a2dp_sink;
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;


void setup() {
    Serial.begin(115200);
    bufferSetup();
    wifiSetup();
    bluetoothSetup();
    irSetup();
    webServerSetup();
    webSocketSetup();
    initializeVariables();
}

void loop() {
//Serial read for restart
    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }
    ackCheck();
    ackReset();
    initDebug();
    currentHeap();
}







