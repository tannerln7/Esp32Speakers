//
// Created by Tanner on 10/22/2023.
//

#include "IR.h"
#include "Handlers.h"

void handleIRCode(uint32_t code) {
    Serial.println(code, HEX);
    switch (code) {
        case volumeUpCode:
            if (currentLinearVolume < MAX_LINEAR_VOLUME) {
                currentLinearVolume++;
                volume(currentLinearVolume);
                Serial.println("Volume Up: " + String(currentLinearVolume));
            } else {
                volume(currentLinearVolume);
                Serial.println("Volume already at max.");
            }
            break;
        case volumeDownCode:
            if (currentLinearVolume > MIN_LINEAR_VOLUME) {
                currentLinearVolume--;
                volume(currentLinearVolume);
                Serial.println("Volume Down: " + String(currentLinearVolume));
            } else {
                volume(currentLinearVolume);
                Serial.println("Volume already at min");
            }
            break;
        case muteCode:
            if (muteState == 0) {
                mute(1);
                muteState = 1;
                Serial.println("Mute ON");
            } else {
                mute(0);
                muteState = 0;
                Serial.println("Mute OFF");
            }
            break;
        case subUpCode:
            if (currentLinearSubVolume < MAX_LINEAR_VOLUME) {
                currentLinearSubVolume++;
                sub(currentLinearSubVolume);
                Serial.println("Sub Volume Up: " + String(currentLinearSubVolume));
            } else {
                sub(currentLinearSubVolume);
                Serial.println("Sub Volume already at max");
            }
            break;
        case subDownCode:
            if (currentLinearSubVolume > MIN_LINEAR_VOLUME) {
                currentLinearSubVolume--;
                sub(currentLinearSubVolume);
                Serial.println("Sub Volume Down: " + String(currentLinearSubVolume));
            } else {
                sub(currentLinearSubVolume);
                Serial.println("Sub volume already at min");
            }
            break;
        case sourceCode:
            if (currentSource == 1) {
                source(2);
                Serial.println("Changed to source 2 (Line in)");
            } else if (currentSource == 2) {
                source(1);
                Serial.println("Changed to source 1 (Optical)");
            }
            break;
        default:
            Serial.println("Unknown Code");
            break;
    }
}