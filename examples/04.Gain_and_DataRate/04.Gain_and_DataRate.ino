//////////////////////////////////////////////////////////////////////////////////////////
//
//    04.Gain_and_DataRate - Programmable gain (PGA) and output data rate
//
//    Demonstrates the two settings that trade resolution, range, and speed:
//
//      - FSR (full-scale range / PGA gain): a smaller range gives a smaller
//        LSB (finer resolution) but clips sooner. getLSBmV() reports the LSB
//        size for whatever range is active.
//      - DR (data rate): 128 SPS .. 3300 SPS. Lower rates average more and are
//        quieter; higher rates are faster.
//
//    The sketch sweeps every FSR setting on AIN0 (single-ended) and prints the
//    LSB size and the reading, then repeats. Tie AIN0 to a steady test voltage
//    (e.g. a divider) to see the same input reported across ranges.
//
//    Author: Ashwin K Whitchurch
//    Copyright (c) 2022 ProtoCentral Electronics
//    Licensed under the MIT License.
//
/////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <protocentral_TLA20xx.h>

TLA20XX adc(TLA20XX_I2C_ADDR_VDD);   // 0x49 (tinyADC default); change the TLA20XX_I2C_ADDR_* constant for your board

struct Range {
    TLA20XX::FSR fsr;
    const char  *label;
};

const Range ranges[6] = {
    { TLA20XX::FSR_6_144V, "+/-6.144V" },
    { TLA20XX::FSR_4_096V, "+/-4.096V" },
    { TLA20XX::FSR_2_048V, "+/-2.048V" },
    { TLA20XX::FSR_1_024V, "+/-1.024V" },
    { TLA20XX::FSR_0_512V, "+/-0.512V" },
    { TLA20XX::FSR_0_256V, "+/-0.256V" },
};

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
    adc.setDR(TLA20XX::DR_490SPS);
    adc.setMux(TLA20XX::MUX_AIN0_GND);
}

void loop()
{
    Serial.println(F("range    \tLSB(mV)\traw\tvolts"));

    for (uint8_t i = 0; i < 6; i++) {
        adc.setFSR(ranges[i].fsr);
        delay(5);   // let a fresh conversion complete after the gain change

        Serial.print(ranges[i].label);
        Serial.print('\t');
        Serial.print(adc.getLSBmV(), 3);
        Serial.print('\t');
        Serial.print(adc.readADC());
        Serial.print('\t');
        Serial.println(adc.readVoltage(), 4);
    }

    Serial.println();
    delay(2000);
}
