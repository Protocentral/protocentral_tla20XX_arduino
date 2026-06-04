//////////////////////////////////////////////////////////////////////////////////////////
//
//    01.SingleEnded_Read - Read one single-ended channel in volts
//
//    Measures AIN0 referenced to GND on a TLA2024 and prints the raw counts
//    alongside the converted voltage. Single-ended inputs accept 0 V .. +FSR.
//
//    With the default +/-2.048 V range, 1 LSB = 1 mV.
//
//    NOTE: the default constructor uses 0x49 (the tinyADC's ADDR->VDD strap).
//          See the README address table if unsure of your board.s address.
//
//    Author: Ashwin K Whitchurch
//    Copyright (c) 2022 ProtoCentral Electronics
//    Licensed under the MIT License.
//
/////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <protocentral_TLA20xx.h>

TLA20XX adc(TLA20XX_I2C_ADDR_VDD);   // 0x49 (tinyADC default); change the TLA20XX_I2C_ADDR_* constant for your board

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Wire.begin();

    if (!adc.begin()) {
        Serial.println(F("TLA20xx not found - check wiring/address"));
        while (1) delay(1000);
    }

    adc.setMode(TLA20XX::OP_CONTINUOUS);
    adc.setDR(TLA20XX::DR_128SPS);
    adc.setFSR(TLA20XX::FSR_2_048V);   // 1 mV / LSB
    adc.setMux(TLA20XX::MUX_AIN0_GND); // single-ended AIN0 vs GND

    Serial.println(F("raw\tvolts"));
}

void loop()
{
    int16_t raw = adc.readADC();

    Serial.print(raw);
    Serial.print('\t');
    Serial.println(adc.readVoltage(), 4);

    delay(100);
}
