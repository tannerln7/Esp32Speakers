//
// Created by Tanner on 12/17/2023.
//

#ifndef ESP32SPEAKERS_WEBSOCKET_H
#define ESP32SPEAKERS_WEBSOCKET_H
#include <Arduino.h>
#include <WebSocketsServer.h>
#include <AsyncTCP.h>
#include <BluetoothA2DPSink.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

extern WebSocketsServer webSocket;
extern String wsUrl;
extern BluetoothA2DPSink a2dpSink;
extern PubSubClient client;


void hexdump(const void *mem, uint32_t len, uint8_t cols);
void audioDataCallback(const uint8_t* data, uint32_t len);
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void data_received_callback();



#endif //ESP32SPEAKERS_WEBSOCKET_H
