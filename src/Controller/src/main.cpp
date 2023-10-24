#include "INCLUDE.h"
#include "Loops.h"
#include "AudioBuffer.h"

AsyncWebServer server(WEBSOCKET_PORT);
AsyncWebSocket webserv("/audio");
BluetoothA2DPSink a2dp_sink;
IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;
AudioBuffer *audioBuffer = nullptr;
unsigned long sentLast = 0;
unsigned sendDelay = 10000;

void setup() {
    Serial.begin(115200);
//    if (esp_spiram_is_initialized()) {
//        audioBuffer = new AudioBuffer(10);
//        Serial.println("SPIRAM initialized");
//    } else Serial.println("SPIRAM not initialized");
    //wifiSetup();
    //bluetoothSetup();
    //irSetup();
    //webServerSetup();
    //webSocketSetup();
    irrecv.enableIRIn();
}

void loop() {
//Serial read for restart
    if (Serial.available()) {
        char c = char(Serial.read());
        if (c == 'R') {
            ESP.restart();
        }
    }

    if (irrecv.decode(&results)) {
        Serial.println(results.value, HEX);
        handleIRCode(long(results.value));
    }
    if (millis() >= sentLast + sendDelay) {
        sentLast = millis();
        Serial.println("Still Looping");
    }
    irrecv.resume();
    //ackCheck();
    //ackReset();
    //initDebug();
    //currentHeap();
}







