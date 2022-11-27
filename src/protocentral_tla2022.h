// ______          _        _____            _             _ 
// | ___ \        | |      /  __ \          | |           | |
// | |_/ / __ ___ | |_ ___ | /  \/ ___ _ __ | |_ _ __ __ _| |
// |  __/ '__/ _ \| __/ _ \| |    / _ \ '_ \| __| '__/ _` | |
// | |  | | | (_) | || (_) | \__/\  __/ | | | |_| | | (_| | |
// \_|  |_|  \___/ \__\___/ \____/\___|_| |_|\__|_|  \__,_|_|

//////////////////////////////////////////////////////////////////////////////////////////
//
//  Arduino Library for the TI TLA2022 ADC (https://www.ti.com/product/TLA2022)
//
//  Copyright (c) 2022 ProtoCentral
//
//  This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
/////////////////////////////////////////////////////////////////////////////////////////

#ifndef protocentral_tla2022_h
#define protocentral_tla2022_h

#include <Arduino.h>
#include <Wire.h>

#define TLA2022_CONV_REG 0x00
#define TLA2022_CONF_REG 0x01

class TLA2022
{
  public:
    TLA2022(uint8_t i2c_addr);
    void begin(void);
    float read_adc();

  private:
    void write_reg(uint8_t reg_addr, uint16_t data);
    uint16_t read_reg(uint8_t reg_addr);

    uint8_t _i2c_addr;

    union I2C_data {
        uint8_t u8_bytes[2];
        uint16_t u16_value;
    } _i2c_data;
};

#endif
