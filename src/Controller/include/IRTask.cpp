
#include <Arduino.h>

/*
 * This include defines the actual pin number for pins like IR_RECEIVE_PIN, IR_SEND_PIN for many different boards and architectures
 */
#include "PinDefinitionsAndMore.h"
#include <IRremote.hpp> // include the library
#include "IRTask.h"
#include "Handlers.h"
bool isRepeat = false;
unsigned long firstRepeatTime = 0;
unsigned long lastHandleTime = 0;

void irSetup() {
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    // Start the receiver and if not 3. parameter specified, take LED_BUILTIN pin from the internal boards definition as default feedback LED
    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

    Serial.print(F("Ready to receive IR signals of protocols: "));
    printActiveIRProtocols(&Serial);
    Serial.println(F("at pin " STR(IR_RECEIVE_PIN)));
    xTaskCreate(
            IRReceiverTask,  /* Task function. */
            "IRReceiverTask",  /* String with name of task. */
            10000,  /* Stack size in bytes. */
            nullptr,  /* Parameter passed as input of the task */
            1,  /* Priority of the task. */
            nullptr  /* Task handle. */
    );
}

void IRReceiverTask(void * parameter) {
    for (;;) {
        if (IrReceiver.decode()) {
            IrReceiver.printIRResultShort(&Serial);
            if (IrReceiver.decodedIRData.protocol == UNKNOWN) {
                Serial.println(F("Received noise or an unknown (or not yet enabled) protocol"));
                IrReceiver.printIRResultRawFormatted(&Serial, true);
                isRepeat = false;  // Reset the repeat flag
            } else if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
                if (!isRepeat) {
                    firstRepeatTime = millis();
                    isRepeat = true;
                }
                if (millis() - firstRepeatTime > 750) {
                    if (millis() - lastHandleTime > 350) {  // 350ms debounce
                        handleIRCode(0);
                        lastHandleTime = millis();  // Update the last handle time
                    }
                }
            }else if (IrReceiver.decodedIRData.decodedRawData == 0xFD02E718) {
                isRepeat = false;  // Reset the repeat flag
                handleIRCode(0xE21DEF00);
            }else if (IrReceiver.decodedIRData.decodedRawData == 0xFC03E718) {
                isRepeat = false;  // Reset the repeat flag
                handleIRCode(0xE51AEF00);
            }else if (IrReceiver.decodedIRData.decodedRawData == 0xF609E718) {
                isRepeat = false;  // Reset the repeat flag
                handleIRCode(0xFC03EF00);
            } else {
                isRepeat = false;  // Reset the repeat flag
                handleIRCode(IrReceiver.decodedIRData.decodedRawData);
            }
            IrReceiver.resume(); // Enable receiving of the next value
        }
        vTaskDelay(10 / portTICK_PERIOD_MS); // Add a small delay, adjust as needed
    }
}
