#include <Wire.h>
#include <SigmaDSP.h>
#include <dspSetup.h>
#include <webSocketSetup.h>
#include <Arduino.h>
#include <Loops.h>
#include <ArduinoWebsockets.h>

using namespace websockets;

#define   MESH_PREFIX     "LivingRoom"
#define   MESH_PASSWORD   "PassworD1234"
#define   MESH_PORT       5555


extern SigmaDSP* dsp;
extern DSPEEPROM* ee;
WebsocketsClient client;

void setup() {
  Serial.begin(115200);
  dspSetup();
  webSocketSetup();
}

void loop() {
    Serial.begin(115200);
    delay(1000);
    ackLoop();
    heartBeatLoop();
    if(client.available()) {
        client.poll();
        Serial.println("Client available");
    }
    delay(500);
}