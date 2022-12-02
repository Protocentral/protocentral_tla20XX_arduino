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

void TLA2022::write_reg(uint8_t reg_addr, uint16_t in_data)
{
    _i2c_data.u16_value = in_data;

	Wire.beginTransmission(_i2c_addr);  

	Wire.write(reg_addr);           
	Wire.write(_i2c_data.u8_bytes[1]);
    Wire.write(_i2c_data.u8_bytes[0]);     

	Wire.endTransmission();           
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

void TLA2022::setFSR(TLA2022::FSR range) {
    uint16_t conf = read_reg(TLA2022_CONF_REG);

    conf &= ~0x0E00;
    conf |= range << 9;

    write_reg(TLA2022_CONF_REG,conf);
}

void TLA2022::setMode(TLA2022::MODE mode) {
    uint16_t conf = read_reg(TLA2022_CONF_REG);

    conf &= ~(1 << 8);
    if (mode == OP_SINGLE) {
        conf |= (1 << 8);
    }
    write_reg(TLA2022_CONF_REG,conf);
}

void TLA2022::setDR(TLA2022::DR rate) {
    uint16_t conf = read_reg(TLA2022_CONF_REG);

    conf |= rate << 5;
    write_reg(TLA2022_CONF_REG,conf);
}

int16_t TLA2022::read_adc() {
    uint16_t in_data = read_reg(TLA2022_CONV_REG);

    int16_t ret = (int16_t) in_data;
    ret>>=4;

    return ret;
}

void TLA2022::begin() 
{
    uint16_t init = read_reg(TLA2022_CONF_REG);

    Serial.print("Inited: ");
    Serial.println(init);

    write_reg(TLA2022_CONF_REG,0x8683);
}


