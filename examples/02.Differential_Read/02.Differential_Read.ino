//////////////////////////////////////////////////////////////////////////////////////////
//
//    02.Differential_Read - Read a differential input in millivolts
//
//    Measures the differential voltage between AIN0 (+) and AIN1 (-). This is
//    the only input mode available on the single-channel TLA2021/TLA2022, and
//    is also supported on the TLA2024.
//
//    Differential readings are signed: AIN0 below AIN1 returns a negative value.
//    A small PGA range (here +/-0.256 V) gives the finest resolution for low
//    level signals such as bridge sensors and thermocouples.
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
    adc.setDR(TLA20XX::DR_250SPS);
    adc.setFSR(TLA20XX::FSR_0_256V);    // 0.125 mV / LSB, best resolution
    adc.setMux(TLA20XX::MUX_AIN0_AIN1); // differential AIN0 - AIN1

    Serial.println(F("raw\tmillivolts"));
}

void loop()
{
    int16_t raw = adc.readADC();

    Serial.print(raw);
    Serial.print('\t');
    Serial.println(adc.readMillivolts(), 3);

    delay(100);
}
