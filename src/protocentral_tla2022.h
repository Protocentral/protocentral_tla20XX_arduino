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

#ifndef protocentral_max30001_h
#define protocentral_max30001_h

#include <Arduino.h>

typedef enum
{
  SAMPLINGRATE_128 = 128,
  SAMPLINGRATE_256 = 256,
  SAMPLINGRATE_512 = 512
} sampRate;

class TLA2022
{
  public:
    TLA2022::TLA2022(uint8_t i2caddr);
    void begin(void);
 
  private:
    void    I2CwriteByte(uint8_t address, uint8_t subAddress, uint8_t data);
    uint8_t I2CreadByte(uint8_t address, uint8_t subAddress);
    void    I2CreadBytes(uint8_t address, uint8_t subAddress, uint8_t * dest, uint8_t count);

    uint8_t _i2caddr;

    union I2C_data {
        uint8_t u8_bytes[2];
        uint16_t u16_value;
    } _i2c_data;
};


#endif
