//
// Created by Tanner on 10/30/2023.
//

#include "dspSetup.h"

extern SigmaDSP dsp;

void dspSetup(){
    //DSP
    Wire.begin(4,33);  // Ensure Wire is initialized//ee = new DSPEEPROM(Wire, EEPROM_I2C_ADDRESS, 256, -1);
    dsp.begin();
    //ee->begin();

    delay(2000);

    Serial.println(F("Pinging i2c lines...\n0 -> device is present\n2 -> device is not present"));
    Serial.print(F("DSP response: "));
    Serial.println(dsp.ping());
    //Serial.print(F("EEPROM ping: "));
    //Serial.println(ee->ping());

    //ee->writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), 1);

    Serial.print(F("\nLoading DSP program... "));
    loadProgram(dsp);
    Serial.println("Done!\n");
}
