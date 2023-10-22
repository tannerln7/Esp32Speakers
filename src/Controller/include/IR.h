//
// Created by Tanner on 10/22/2023.
//

#ifndef ESP32SPEAKERS_IR_H
#define ESP32SPEAKERS_IR_H

#include <Arduino.h>
#include "DEFINES.h"
#include "IRrecv.h"

extern decode_results results;
extern IRrecv irrecv;

const uint32_t volumeUpCode = 0xF7B847;
const uint32_t volumeDownCode = 0xF758A7;
const uint32_t muteCode = 0xF7C03F;
const uint32_t subUpCode = 0xF738C7;
const uint32_t subDownCode = 0xF77887;
const uint32_t sourceCode = 0xF728D7;
const uint64_t repeatCode = 0xFFFFFFFFFFFFFF;

unsigned long lastIRReceivedTime = 0;
long lastOperation = 0;

void handleIRCode(uint32_t code);


void IRReceiverTask(void * pvParameters) {
    // Initialize your IR receiver here
    while (true) {
        if (irrecv.decode(&results)) {
            unsigned long currentIRReceivedTime = millis();
            // If it's a repeat code, replay the last operation
            if (results.value == repeatCode && lastOperation != 0) {
                if (currentIRReceivedTime - lastIRReceivedTime > debounceDelay) {
                    handleIRCode(lastOperation);
                    Serial.println("Repeat Code" + String(lastOperation));
                }
            } else {
                // If it's not a repeat code and enough time has passed (debouncing)
                if (currentIRReceivedTime - lastIRReceivedTime > debounceDelay) {
                    lastOperation = (long) results.value;
                    handleIRCode(results.value);
                    Serial.println(results.value, HEX);
                }
            }
            lastIRReceivedTime = currentIRReceivedTime;
            irrecv.resume();
        }
        // Use delay to free up the core for other tasks
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


#endif //ESP32SPEAKERS_IR_H
