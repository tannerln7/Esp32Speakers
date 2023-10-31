//
// Created by Tanner on 10/30/2023.
//

#include "dspSetup.h"
SigmaDSP* dsp;
DSPEEPROM* ee;

void dspSetup(){
    //DSP
    Wire.begin();  // Ensure Wire is initialized
    dsp = new SigmaDSP(Wire, DSP_I2C_ADDRESS, 48000.00f);
    ee = new DSPEEPROM(Wire, EEPROM_I2C_ADDRESS, 256, -1);
    Wire.begin();
    dsp->begin();
    ee->begin();

    delay(2000);

    Serial.println(F("Pinging i2c lines...\n0 -> device is present\n2 -> device is not present"));
    Serial.print(F("DSP response: "));
    Serial.println(dsp->ping());
    Serial.print(F("EEPROM ping: "));
    Serial.println(ee->ping());

    ee->writeFirmware(DSP_eeprom_firmware, sizeof(DSP_eeprom_firmware), 0);
    dsp->reset();
    delay(2000);
}
