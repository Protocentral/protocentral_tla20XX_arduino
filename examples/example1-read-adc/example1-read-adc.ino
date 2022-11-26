#include <Arduino.h>
#include <Wire.h>

#include <protocentral_tla2022.h>

TLA2022 tla2022(0x01);

void setup() {

    Serial.begin(115200);
    Serial.println("Starting ADC...");

    Wire.begin();

    tla2022.begin();

    /*if (adc.begin()) {
        Serial.println("Device is init-ed");
    }
    else {
        Serial.println("Device is not init-ed. Continue anyway...");
    }

//    adc.setMuxConfig(TLA2024::MUX_AIN2_GND);

    /*adc.setFullScaleRange(TLA2024::FSR_2_048V);
    adc.setDataRate(TLA2024::DR_3300SPS);
    adc.setOperatingMode(TLA2024::OP_SINGLE);*/
}

void loop() {

    float val = tla2022.read_adc(); //use this in conjunction with adc.setMuxConfig() method;

    //float val = adc.analogRead(channelToRead);
    //Serial.printf("Channel %d, ADC reading: %.3f\n", channelToRead, val);

//    long converted = map((long)val, -2048, 2047, 0, 2048 );
//    float convF = 2*converted*1.0f/1000;
//    Serial.printf("V = %.3f\n", convF);

    //Serial.printf("Voltage = %.3fV\n", adc.voltageRead(channelToRead));
    Serial.println();

    delay(1000);
}