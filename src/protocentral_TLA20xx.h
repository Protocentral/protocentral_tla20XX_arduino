//////////////////////////////////////////////////////////////////////////////////////////
//
//    Arduino library for the ProtoCentral tinyADC TLA20xx breakout board
//
//    The Texas Instruments TLA2021/TLA2022/TLA2024 are 12-bit, low-power,
//    delta-sigma analog-to-digital converters with an integrated voltage
//    reference and a programmable gain amplifier (PGA), in a tiny package.
//
//      - TLA2021 : 1 single differential channel, fixed FSR (no PGA, no MUX)
//      - TLA2022 : 1 single differential channel, programmable PGA (no MUX)
//      - TLA2024 : 4 single-ended / 2 differential channels, MUX + programmable PGA
//
//    All three are register-compatible (the TLA2024 simply implements the MUX
//    and PGA fields the smaller parts ignore), so this single class drives the
//    whole family. Programmable data rate from 128 SPS to 3.3 kSPS.
//
//    Author: Ashwin K Whitchurch
//    Copyright (c) 2022 ProtoCentral Electronics
//
//    This software is licensed under the MIT License (http://opensource.org/licenses/MIT).
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//    INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//    PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//    OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//    For information on how to use, visit https://github.com/protocentral/protocentral_tla20XX_arduino
//
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef _PROTOCENTRAL_TLA20XX_H_
#define _PROTOCENTRAL_TLA20XX_H_

#include <Arduino.h>
#include <Wire.h>

// =============================================================================
// I2C addresses (selected by the ADDR pin)
// =============================================================================
#define TLA20XX_I2C_ADDR_GND        (0x48)  // ADDR -> GND
#define TLA20XX_I2C_ADDR_VDD        (0x49)  // ADDR -> VDD
#define TLA20XX_I2C_ADDR_SDA        (0x4A)  // ADDR -> SDA
#define TLA20XX_I2C_ADDR_SCL        (0x4B)  // ADDR -> SCL

// ProtoCentral tinyADC ships with ADDR tied to VDD (0x49).
#define TLA20XX_DEFAULT_ADDRESS     TLA20XX_I2C_ADDR_VDD

// =============================================================================
// Register map
// =============================================================================
#define TLA20XX_REG_CONVERSION      (0x00)  // 12-bit result, read-only
#define TLA20XX_REG_CONFIG          (0x01)  // 16-bit configuration

// =============================================================================
// CONFIG register bit fields
// =============================================================================
#define TLA20XX_OS_MASK             (0x8000)  // bit 15
#define TLA20XX_OS_START            (0x8000)  // write 1: start a single conversion
#define TLA20XX_OS_READY            (0x8000)  // read 1: idle (conversion complete)

#define TLA20XX_MUX_MASK            (0x7000)  // bits 14:12 (TLA2024 only)
#define TLA20XX_MUX_SHIFT           (12)

#define TLA20XX_PGA_MASK            (0x0E00)  // bits 11:9 (TLA2022/2024)
#define TLA20XX_PGA_SHIFT           (9)

#define TLA20XX_MODE_MASK           (0x0100)  // bit 8
#define TLA20XX_MODE_SHIFT          (8)

#define TLA20XX_DR_MASK             (0x00E0)  // bits 7:5
#define TLA20XX_DR_SHIFT            (5)

// Bits [4:0] are non-functional on the TLA20xx (no comparator). The datasheet
// power-on default leaves them at 0b00011; preserve that.
#define TLA20XX_CONFIG_RESERVED     (0x0003)

// Library default: continuous mode, AIN0<->AIN1, FSR +/-2.048 V, 1600 SPS.
#define TLA20XX_CONFIG_DEFAULT      (0x8483)

class TLA20XX
{
  public:
    // -------------------------------------------------------------- data rate
    enum DR {
        DR_128SPS  = 0x00,
        DR_250SPS  = 0x01,
        DR_490SPS  = 0x02,
        DR_920SPS  = 0x03,
        DR_1600SPS = 0x04,
        DR_2400SPS = 0x05,
        DR_3300SPS = 0x06,
    };

    // ------------------------------------------- full-scale range (PGA gain)
    enum FSR {
        FSR_6_144V = 0x00,   // 3     mV / LSB
        FSR_4_096V = 0x01,   // 2     mV / LSB
        FSR_2_048V = 0x02,   // 1     mV / LSB  (default)
        FSR_1_024V = 0x03,   // 0.5   mV / LSB
        FSR_0_512V = 0x04,   // 0.25  mV / LSB
        FSR_0_256V = 0x05,   // 0.125 mV / LSB
    };

    // ------------------------------------------------ input multiplexer (TLA2024)
    enum MUX {
        MUX_AIN0_AIN1 = 0x00,   // differential (default)
        MUX_AIN0_AIN3 = 0x01,   // differential
        MUX_AIN1_AIN3 = 0x02,   // differential
        MUX_AIN2_AIN3 = 0x03,   // differential
        MUX_AIN0_GND  = 0x04,   // single-ended
        MUX_AIN1_GND  = 0x05,   // single-ended
        MUX_AIN2_GND  = 0x06,   // single-ended
        MUX_AIN3_GND  = 0x07,   // single-ended
    };

    // -------------------------------------------------------- conversion mode
    enum MODE {
        OP_CONTINUOUS = 0,   // free-running conversions
        OP_SINGLE     = 1,   // single-shot, powers down between conversions
    };

    /**
     * @brief Construct a driver for a TLA2021/2022/2024.
     * @param i2c_addr  7-bit I2C address (default 0x49, the tinyADC's ADDR->VDD strap).
     * @param wire      I2C bus to use (default Wire).
     */
    TLA20XX(uint8_t i2c_addr = TLA20XX_DEFAULT_ADDRESS, TwoWire *wire = &Wire);

    // ------------------------------------------------------------- lifecycle
    bool      begin();            // probe the bus, load default config
    bool      isConnected();      // true if the device ACKs at its address

    // -------------------------------------------------------- configuration
    void      setFSR(FSR fsr);    // programmable gain / full-scale range
    void      setMode(MODE mode); // continuous vs single-shot
    void      setDR(DR rate);     // output data rate
    void      setMux(MUX mux);    // input multiplexer (TLA2024)

    FSR       getFSR();
    MODE      getMode();
    DR        getDR();
    MUX       getMux();
    uint16_t  getConfig() const { return _config; }

    // ------------------------------------------------------------ continuous
    int16_t   readADC();          // signed 12-bit counts from the current input
    float     readMillivolts();   // counts scaled by the active FSR
    float     readVoltage();      // millivolts / 1000

    // ------------------------------------------------------------ single-shot
    void      startSingleConversion();  // trigger one conversion (OS = 1)
    bool      isConversionReady();      // true once the conversion has completed
    int16_t   readADC_SingleShot();     // trigger, wait, and return the result

    // ----------------------------------------------------------- conversions
    float     getLSBmV();         // millivolts per LSB for the active FSR
    float     toMillivolts(int16_t counts);

    // -------------------------------------------------- raw register access
    uint16_t  readRegister(uint8_t reg_addr);
    void      writeRegister(uint8_t reg_addr, uint16_t data);

    // ------------------------------------------------ deprecated (pre-2.0 API)
    int16_t   read_adc() { return readADC(); }  // use readADC()

  private:
    uint8_t   _i2c_addr;
    TwoWire  *_wire;
    uint16_t  _config;   // cached CONFIG register so setters update one field

    void      writeConfig();
};

#endif // _PROTOCENTRAL_TLA20XX_H_
