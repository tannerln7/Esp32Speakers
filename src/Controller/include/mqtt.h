//
// Created by LANETAN on 12/19/2023.
//

#ifndef SPEAKERS_MQTT_H
#define SPEAKERS_MQTT_H

#include <PubSubClient.h>
#include <Arduino.h>

extern PubSubClient client;

void reconnect();
void mqttCallback(const char* topic, byte* payload, unsigned int length);


#endif //SPEAKERS_MQTT_H
