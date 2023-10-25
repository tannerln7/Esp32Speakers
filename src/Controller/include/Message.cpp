//
// Created by Tanner on 10/22/2023.
//

#include "Message.h"




//TODO: add a way to send a message to a specific client
__attribute__((unused)) void sendToAll(const String &data) {
    webserv.textAll(data);
}
//TODO: add method to send completed audio packets to all clients
void nodeTimeAdjustedCallback(int32_t offset) {
    //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}