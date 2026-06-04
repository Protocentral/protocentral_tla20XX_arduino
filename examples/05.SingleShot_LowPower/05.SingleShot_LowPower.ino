//////////////////////////////////////////////////////////////////////////////////////////
//
//    05.SingleShot_LowPower - One-shot conversions for low-power sampling
//
//    In single-shot mode the TLA20xx performs exactly one conversion when
//    triggered, then powers itself down until the next trigger. Between samples
//    the ADC draws only its quiescent current, which is ideal for battery
//    powered nodes that read slowly (e.g. once per second).
//
//    readADC_SingleShot() does the whole handshake: set the OS bit to start the
//    conversion, poll OS until it reads back high (conversion complete), then
//    return the result. Here we take one reading per second and sleep the MCU
//    in delay() in between.
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

    adc.setMode(TLA20XX::OP_SINGLE);     // power down between conversions
    adc.setDR(TLA20XX::DR_128SPS);
    adc.setFSR(TLA20XX::FSR_2_048V);
    adc.setMux(TLA20XX::MUX_AIN0_GND);

    Serial.println(F("One-shot sampling at 1 Hz"));
    Serial.println(F("raw\tvolts"));
}

void loop()
{
    int16_t raw = adc.readADC_SingleShot();   // trigger, wait, read
    float   v   = adc.toMillivolts(raw) / 1000.0f;

    Serial.print(raw);
    Serial.print('\t');
    Serial.println(v, 4);

    // ADC is powered down here; only the MCU is awake.
    delay(1000);
}
