// | ___ \        | |      /  __ \          | |           | |
// | |_/ / __ ___ | |_ ___ | /  \/ ___ _ __ | |_ _ __ __ _| |
// |  __/ '__/ _ \| __/ _ \| |    / _ \ '_ \| __| '__/ _` | |
// | |  | | | (_) | || (_) | \__/\  __/ | | | |_| | | (_| | |
// \_|  |_|  \___/ \__\___/ \____/\___|_| |_|\__|_|  \__,_|_|

//////////////////////////////////////////////////////////////////////////////////////////
//
//  Arduino Library for the TI TLA2022 ADC (https://www.ti.com/product/TLA2022)
//
//  Copyright (c) 2020 ProtoCentral
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

#include<SPI.h>
#include "protocentral_tla2022.h"

TLA2022::TLA2022(uint8_t i2c_addr)
{
    _i2c_addr=i2c_addr;

}

// Wire.h read and write protocols
void TLA2022::write_reg(uint8_t reg_addr, uint16_t in_data)
{

    _i2c_data.u16_value = in_data;

	Wire.beginTransmission(_i2c_addr);  

	Wire.write(reg_addr);           
	Wire.write(_i2c_data.u8_bytes[1]);
    Wire.write(_i2c_data.u8_bytes[0]);     

	Wire.endTransmission();           

    /*devI2C_->beginTransmission(addr);
    devI2C_->write(confReg_);
    written += devI2C_->write(data.packet[1]);
    written += devI2C_->write(data.packet[0]);
    devI2C_->endTransmission();
    */
}

uint16_t TLA2022::read_reg(uint8_t reg_addr){
	uint16_t data; 
	
    Wire.beginTransmission(_i2c_addr);

	Wire.write(reg_addr);

	Wire.endTransmission(false);
	Wire.requestFrom(_i2c_addr, (uint8_t) 2);

    _i2c_data.u8_bytes[1]=Wire.read();
    _i2c_data.u8_bytes[0]=Wire.read();

	data = _i2c_data.u16_value;
    
	return data;
}

float TLA2022::read_adc() {
    // this only needs to run when in single shot.
    //if (currentMode_ == OP_SINGLE) {
        // write 1 to OS bit to start conv
        
        /*uint16_t current_conf = read_reg(TLA2022_CONF_REG);
        current_conf |= 0x8000;
        write_reg(TLA2022_CONF_REG,current_conf);
        // OS bit will be 0 until conv is done.
        do {
            delay(5);
        } while ((read_reg(TLA2022_CONF_REG) & 0x8000) == 0);
        */
    //}

    //uint16_t current_conf = read_reg(TLA2022_CONF_REG);
    //Serial.print("CF: ");
    //Serial.println(current_conf, HEX);
    

    // get data from conv_reg
    uint16_t in_data = read_reg(TLA2022_CONV_REG);

    // shift out unused bits
    in_data >>= 4;

    // get sign and mask accordingly
    if (in_data & (1 << 11)) {
        // 11th bit is sign bit. if its set, set bits 15-12
        in_data |= 0xF000;
    } else {
        // not set, clear bits 15-12
        in_data &= ~0xF000;
    }

    // now store it as a signed 16 bit int.
    int16_t ret = in_data;

    // default Full Scale Range is -2.048V to 2.047V.
    // our 12bit 2's complement goes from -2048 to 2047 :)
    // return ret /1000.0;

    // return raw adc data
    return ret;
}

void TLA2022::begin() 
{
    uint16_t init = read_reg(TLA2022_CONF_REG);

    Serial.print("Done init: ");
    Serial.print(init);

    write_reg(TLA2022_CONF_REG,0x8683);
}


