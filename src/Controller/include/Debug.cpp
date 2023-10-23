//
// Created by Tanner on 10/22/2023.
//

#include "Debug.h"

void currentHeap() {
    unsigned long currentMillis = millis();
    unsigned long lastPrintTime = 0;
    const unsigned long DEBOUNCE_DELAY = 3000;

    if (currentMillis - lastPrintTime >= DEBOUNCE_DELAY) {
        // Save the last time we printed the free heap
        lastPrintTime = currentMillis;

        // Print the free heap
        Serial.println("Memory: " + String(ESP.getFreeHeap()));
    }
}