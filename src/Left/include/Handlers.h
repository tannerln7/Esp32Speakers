//
// Created by Tanner on 10/30/2023.
//

#ifndef ESP32SPEAKERS_HANDLERS_H
#define ESP32SPEAKERS_HANDLERS_H
#endif //ESP32SPEAKERS_HANDLERS_H

#include <WiFi.h>
#include <SigmaDSP.h>
#include <DSPEEPROM.h>
#include <SigmaDSP_parameters.h>


void handleCallback(String data, size_t len);
void handleVolumeCommand(String &value1);
void handleSubCommand(String &value1);
void handleMuteCommand(String &value1);
void handleSourceCommand(String &value1);
void handleHeartbeatCommand(String &value1, String &value2, String &value3, String &value4, String &value5, String &value6, String &value7);
void handleAckCommand(String &value1, String &value2, String &value3, String &value4, String &value5, String &value6, String &value7);
void parseIncomingMessage(String &msg, String &command, String &value1, String &value2, String &value3, String &value4,
                          String &value5, String &value6, String &value7);
void setVolume(float);
void setSubVolume(float);
void mute(int);
void changeSource(int);
void sendHeartbeat();
void sendMessage();
String getValue(const String& data, char separator, int index);

