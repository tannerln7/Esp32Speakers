////
//// Created by Tanner on 12/17/2023.
////
//
//#include "websocket.h"
//std::queue<MyMessageType> messageQueue;
//
//void sendWebSocketMessage() {
//    if (!messageQueue.empty() && webSocket.()) {
//        MyMessageType message = messageQueue.front();
//        // send message logic
//        wsClient.sendBinary(message.data, message.len);
//        messageQueue.pop();
//    }
//}
//
//void hexdump(const void *mem, uint32_t len, uint8_t cols = 16) {
//    const uint8_t* src = (const uint8_t*) mem;
//    Serial.printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", (ptrdiff_t)src, len, len);
//    for(uint32_t i = 0; i < len; i++) {
//        if(i % cols == 0) {
//            Serial.printf("\n[0x%08X] 0x%08X: ", (ptrdiff_t)src, i);
//        }
//        Serial.printf("%02X ", *src);
//        src++;
//    }
//    Serial.printf("\n");
//}
//
//void audioDataCallback(const uint8_t* data, uint32_t len) {
//    // Check if there are any connected WebSocket clients
//    if (webSocket.connectedClients() > 0) {
//        webSocket.broadcastTXT(data, len);
//    } else {
//        Serial.println("No WebSocket clients connected.");
//    }
//
//    // For testing: Send a portion of the data over MQTT (ensure it's a reasonable size for MQTT)
//    // Limit the size of the data sent over MQTT for testing to avoid payload size issues
//    //const size_t mqtt_test_size = min(len, size_t(100)); // Example: limit to 100 bytes
//    //client.publish("topic/audio/test", reinterpret_cast<const char*>(data), mqtt_test_size);
//}
//
//void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//
//    switch (type) {
//        case WStype_DISCONNECTED:
//            Serial.printf("[%u] Disconnected!\n", num);
//            break;
//        case WStype_CONNECTED: {
//            IPAddress ip = webSocket.remoteIP(num);
//            Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
//
//            // send message to client
//            webSocket.sendTXT(num, "Connected");
//        }
//            break;
//        case WStype_TEXT:
//            Serial.printf("[%u] get Text: %s\n", num, payload);
//
//            // send message to client
//            // webSocket.sendTXT(num, "message here");
//
//            // send data to all connected clients
//            // webSocket.broadcastTXT("message here");
//            break;
//        case WStype_BIN:
//            Serial.printf("[%u] get binary length: %u\n", num, length);
//            hexdump(payload, length);
//
//            // send message to client
//            // webSocket.sendBIN(num, payload, length);
//            break;
//        case WStype_ERROR:
//        case WStype_FRAGMENT_TEXT_START:
//        case WStype_FRAGMENT_BIN_START:
//        case WStype_FRAGMENT:
//        case WStype_FRAGMENT_FIN:
//            break;
//    }
//}
//
//    void data_received_callback(){
//        Serial.println("Data received");
//}