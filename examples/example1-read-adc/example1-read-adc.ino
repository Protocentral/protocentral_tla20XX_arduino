#include <Arduino.h>
#include <Wire.h>

#include <protocentral_tla2022.h>

#define TLA2022_I2C_ADDR 0x49

TLA2022 tla2022(TLA2022_I2C_ADDR);

void setup() {

    Serial.begin(57600);
    Serial.println("Starting ADC...");

    //Wire.setSDA(4);
    //Wire.setSCL(5);

    Wire.begin();

    tla2022.begin();

    tla2022.setFSR(TLA2022::FSR_1_024V);

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

float val;

void loop() {

    val = tla2022.read_adc(); 

    //float val = adc.analogRead(channelToRead);
    //Serial.printf("Channel %d, ADC reading: %.3f\n", channelToRead, val);

//    long converted = map((long)val, -2048, 2047, 0, 2048 );
//    float convF = 2*converted*1.0f/1000;
//    Serial.printf("V = %.3f\n", convF);

    Serial.println(val);
    //Serial.println("RA");

    delay(100);
}