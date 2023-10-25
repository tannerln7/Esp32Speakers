//
// Created by Tanner on 10/22/2023.
//

#include "Debug.h"
unsigned long currentMillis = millis();
unsigned long lastPrintTime = 0;
const unsigned long DEBOUNCE_DELAY = 3000;
//TODO Debug why this isnt printing. Probably more watchdog issues.
void currentHeap() {

    if (currentMillis - lastPrintTime >= DEBOUNCE_DELAY) {
        // Save the last time we printed the free heap
        lastPrintTime = currentMillis;

        // Print the free heap
        Serial.println("Memory: " + String(ESP.getFreeHeap()));
    }
}