//////////////////////////////////////////////////////////////////////////////////////////
//
//    06.OpenView_Streaming - Stream samples to ProtoCentral OpenView
//
//    Streams two single-ended channels (AIN0 and AIN1) to the ProtoCentral
//    OpenView desktop application using the SAME packet format as the
//    ProtoCentral ADS1292R board (also MAX30001 compatible), so OpenView plots
//    it directly. Select the serial port at 57600 baud in OpenView.
//
//    Packet layout (matches the ADS1292R "CES_CMDIF" data packet):
//      Header  : 0x0A 0xFA  DATA_LEN(0x0C) 0x00  0x02
//      Payload : ch1 int32 LE | ch2 int32 LE | lead-off byte | 3x reserved   (12 bytes)
//      Footer  : 0x00 0x0B
//
//    Channel 1 = AIN0 vs GND, Channel 2 = AIN1 vs GND. The 12-bit ADC reading
//    is sign-extended into the 32-bit channel field that OpenView expects.
//
//    Download OpenView: https://github.com/Protocentral/protocentral_openview
//
//    Author: Ashwin K Whitchurch
//    Copyright (c) 2022 ProtoCentral Electronics
//    Licensed under the MIT License.
//
/////////////////////////////////////////////////////////////////////////////////////////

#include <Wire.h>
#include <protocentral_TLA20xx.h>

TLA20XX adc(TLA20XX_I2C_ADDR_VDD);   // 0x49 (tinyADC default); change the TLA20XX_I2C_ADDR_* constant for your board

// ----- ProtoCentral OpenView packet framing (ADS1292R-compatible) -----
#define CES_CMDIF_PKT_START_1   0x0A
#define CES_CMDIF_PKT_START_2   0xFA
#define CES_CMDIF_TYPE_DATA     0x02
#define CES_CMDIF_PKT_STOP      0x0B
#define DATA_LEN                0x0C   // 12-byte payload, same as the ADS1292R
#define ZERO                    0

uint8_t dataPacket[DATA_LEN];
const uint8_t dataPacketHeader[5] = { CES_CMDIF_PKT_START_1, CES_CMDIF_PKT_START_2,
                                      DATA_LEN, ZERO, CES_CMDIF_TYPE_DATA };
const uint8_t dataPacketFooter[2] = { ZERO, CES_CMDIF_PKT_STOP };

// ~100 Hz output pacing (one 2-channel packet every 10 ms).
const unsigned long SAMPLE_INTERVAL_US = 10000;
unsigned long lastSampleTime = 0;

void setup()
{
    Serial.begin(57600);   // OpenView uses 57600 baud

    Wire.begin();

    adc.begin();
    adc.setMode(TLA20XX::OP_CONTINUOUS);
    adc.setDR(TLA20XX::DR_3300SPS);    // fast, so per-channel MUX settle is short
    adc.setFSR(TLA20XX::FSR_2_048V);
}

// Read one single-ended channel, sign-extended to 32 bits for the packet.
int32_t readChannel(TLA20XX::MUX mux)
{
    adc.setMux(mux);
    delay(1);                          // let one conversion flush after the MUX switch
    return (int32_t)adc.readADC();
}

// Send a 2-channel data packet in ADS1292R-compatible OpenView format.
void sendDataPacket(int32_t ch1, int32_t ch2)
{
    // Channel 1 (4 bytes, LSB first)
    dataPacket[0] = ch1;
    dataPacket[1] = ch1 >> 8;
    dataPacket[2] = ch1 >> 16;
    dataPacket[3] = ch1 >> 24;

    // Channel 2 (4 bytes, LSB first)
    dataPacket[4] = ch2;
    dataPacket[5] = ch2 >> 8;
    dataPacket[6] = ch2 >> 16;
    dataPacket[7] = ch2 >> 24;

    // Lead-off / status byte (no lead detection on the TLA20xx) + reserved
    dataPacket[8]  = 0x00;
    dataPacket[9]  = 0x00;
    dataPacket[10] = 0x00;
    dataPacket[11] = 0x00;

    Serial.write(dataPacketHeader, sizeof(dataPacketHeader));
    Serial.write(dataPacket, DATA_LEN);
    Serial.write(dataPacketFooter, sizeof(dataPacketFooter));
}

void loop()
{
    unsigned long now = micros();
    if (now - lastSampleTime < SAMPLE_INTERVAL_US) return;
    lastSampleTime = now;

    int32_t ch1 = readChannel(TLA20XX::MUX_AIN0_GND);
    int32_t ch2 = readChannel(TLA20XX::MUX_AIN1_GND);

    sendDataPacket(ch1, ch2);
}
