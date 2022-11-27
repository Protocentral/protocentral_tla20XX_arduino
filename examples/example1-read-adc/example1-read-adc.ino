#include <Arduino.h>
#include <Wire.h>

#include <protocentral_tla2022.h>

#define TLA2022_I2C_ADDR 0x49

TLA2022 tla2022(TLA2022_I2C_ADDR);

void setup() 
{
    Serial.begin(57600);
    Serial.println("Starting ADC...");

    //Wire.setSDA(4);
    //Wire.setSCL(5);

    Wire.begin();

    tla2022.begin();

    tla2022.setFSR(TLA2022::FSR_1_024V);
    tla2022.setMode(TLA2022::OP_CONTINUOUS);
    tla2022.setDR(TLA2022::DR_128SPS);
}

float val;

void loop() 
{
    val = tla2022.read_adc(); 

    Serial.println(val);

    delay(100);
}