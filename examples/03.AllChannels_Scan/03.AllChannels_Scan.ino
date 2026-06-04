//////////////////////////////////////////////////////////////////////////////////////////
//
//    03.AllChannels_Scan - Scan all four single-ended channels (TLA2024)
//
//    The TLA2024 has a single ADC core fed by an input multiplexer, so the
//    four single-ended channels are read by switching the MUX and taking one
//    conversion per channel. This sketch walks AIN0..AIN3 vs GND each loop and
//    prints all four voltages on one line.
//
//    Requires a TLA2024 (the TLA2021/2022 expose only AIN0/AIN1).
//
//    Author: Ashwin K Whitchurch
//    Copyright (c) 2022 ProtoCentral Electronics
//    Licensed under the MIT License.
//
/////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <protocentral_TLA20xx.h>

TLA20XX adc(TLA20XX_I2C_ADDR_VDD);   // 0x49 (tinyADC default); change the TLA20XX_I2C_ADDR_* constant for your board

// Single-ended channels in scan order.
const TLA20XX::MUX channels[4] = {
    TLA20XX::MUX_AIN0_GND,
    TLA20XX::MUX_AIN1_GND,
    TLA20XX::MUX_AIN2_GND,
    TLA20XX::MUX_AIN3_GND,
};

void setup()
{
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Wire.begin();

    if (!adc.begin()) {
        Serial.println(F("TLA2024 not found - check wiring/address"));
        while (1) delay(1000);
    }

    adc.setMode(TLA20XX::OP_CONTINUOUS);
    adc.setDR(TLA20XX::DR_3300SPS);   // fast, so the per-channel settle is short
    adc.setFSR(TLA20XX::FSR_4_096V);  // 0 .. 4.096 V single-ended span

    Serial.println(F("AIN0\tAIN1\tAIN2\tAIN3   (volts)"));
}

void loop()
{
    for (uint8_t i = 0; i < 4; i++) {
        adc.setMux(channels[i]);
        // After switching the MUX, allow one conversion to flush before reading.
        delay(2);
        Serial.print(adc.readVoltage(), 3);
        Serial.print(i < 3 ? '\t' : '\n');
    }

    delay(200);
}
