//////////////////////////////////////////////////////////////////////////////////////////
//
//    Arduino library for the ProtoCentral tinyADC TLA20xx breakout board
//
//    Author: Ashwin K Whitchurch
//    Copyright (c) 2022 ProtoCentral Electronics
//
//    This software is licensed under the MIT License (http://opensource.org/licenses/MIT).
//
/////////////////////////////////////////////////////////////////////////////////////////

#include "protocentral_TLA20xx.h"

TLA20XX::TLA20XX(uint8_t i2c_addr, TwoWire *wire)
    : _i2c_addr(i2c_addr),
      _wire(wire),
      _config(TLA20XX_CONFIG_DEFAULT)
{
}

// ============================================================================
// Lifecycle
// ============================================================================

bool TLA20XX::begin()
{
    if (!isConnected()) return false;

    // Load a known-good configuration: continuous mode, AIN0<->AIN1,
    // +/-2.048 V, 1600 SPS.
    _config = TLA20XX_CONFIG_DEFAULT;
    writeConfig();
    return true;
}

bool TLA20XX::isConnected()
{
    // The TLA20xx has no ID register, so probe for an I2C ACK.
    _wire->beginTransmission(_i2c_addr);
    return (_wire->endTransmission() == 0);
}

// ============================================================================
// Configuration  (read-modify-write a single field of the cached CONFIG word)
// ============================================================================

void TLA20XX::setFSR(FSR fsr)
{
    _config &= ~TLA20XX_PGA_MASK;
    _config |= ((uint16_t)fsr << TLA20XX_PGA_SHIFT) & TLA20XX_PGA_MASK;
    writeConfig();
}

void TLA20XX::setMode(MODE mode)
{
    _config &= ~TLA20XX_MODE_MASK;
    if (mode == OP_SINGLE) {
        _config |= TLA20XX_MODE_MASK;
    }
    writeConfig();
}

void TLA20XX::setDR(DR rate)
{
    _config &= ~TLA20XX_DR_MASK;
    _config |= ((uint16_t)rate << TLA20XX_DR_SHIFT) & TLA20XX_DR_MASK;
    writeConfig();
}

void TLA20XX::setMux(MUX mux)
{
    _config &= ~TLA20XX_MUX_MASK;
    _config |= ((uint16_t)mux << TLA20XX_MUX_SHIFT) & TLA20XX_MUX_MASK;
    writeConfig();
}

TLA20XX::FSR TLA20XX::getFSR()
{
    return (FSR)((_config & TLA20XX_PGA_MASK) >> TLA20XX_PGA_SHIFT);
}

TLA20XX::MODE TLA20XX::getMode()
{
    return (_config & TLA20XX_MODE_MASK) ? OP_SINGLE : OP_CONTINUOUS;
}

TLA20XX::DR TLA20XX::getDR()
{
    return (DR)((_config & TLA20XX_DR_MASK) >> TLA20XX_DR_SHIFT);
}

TLA20XX::MUX TLA20XX::getMux()
{
    return (MUX)((_config & TLA20XX_MUX_MASK) >> TLA20XX_MUX_SHIFT);
}

// ============================================================================
// Continuous-mode acquisition
// ============================================================================

int16_t TLA20XX::readADC()
{
    uint16_t raw = readRegister(TLA20XX_REG_CONVERSION);

    // The 12-bit two's-complement result is left-justified in bits [15:4].
    // Casting to signed before the shift sign-extends negative readings.
    return (int16_t)raw >> 4;
}

float TLA20XX::readMillivolts()
{
    return readADC() * getLSBmV();
}

float TLA20XX::readVoltage()
{
    return readMillivolts() / 1000.0f;
}

// ============================================================================
// Single-shot acquisition (low power: the device powers down between samples)
// ============================================================================

void TLA20XX::startSingleConversion()
{
    // Force single-shot mode for this conversion and set OS to start it,
    // without disturbing the cached configuration.
    uint16_t cfg = _config | TLA20XX_MODE_MASK | TLA20XX_OS_START;
    writeRegister(TLA20XX_REG_CONFIG, cfg);
}

bool TLA20XX::isConversionReady()
{
    // OS reads back high once the conversion completes (single-shot mode).
    return (readRegister(TLA20XX_REG_CONFIG) & TLA20XX_OS_MASK) != 0;
}

int16_t TLA20XX::readADC_SingleShot()
{
    startSingleConversion();
    while (!isConversionReady()) {
        // Conversion time is 1/DR; this poll typically returns in well under a ms.
    }
    return readADC();
}

// ============================================================================
// Conversions
// ============================================================================

float TLA20XX::getLSBmV()
{
    switch (getFSR()) {
        case FSR_6_144V: return 3.0f;
        case FSR_4_096V: return 2.0f;
        case FSR_2_048V: return 1.0f;
        case FSR_1_024V: return 0.5f;
        case FSR_0_512V: return 0.25f;
        case FSR_0_256V: return 0.125f;
        default:         return 1.0f;
    }
}

float TLA20XX::toMillivolts(int16_t counts)
{
    return counts * getLSBmV();
}

// ============================================================================
// Raw register access (big-endian on the wire)
// ============================================================================

uint16_t TLA20XX::readRegister(uint8_t reg_addr)
{
    _wire->beginTransmission(_i2c_addr);
    _wire->write(reg_addr);
    if (_wire->endTransmission(false) != 0) {
        _wire->endTransmission();
        return 0;
    }

    if (_wire->requestFrom(_i2c_addr, (uint8_t)2) != 2) return 0;
    uint16_t hi = _wire->read();
    uint16_t lo = _wire->read();
    return (hi << 8) | lo;
}

void TLA20XX::writeRegister(uint8_t reg_addr, uint16_t data)
{
    _wire->beginTransmission(_i2c_addr);
    _wire->write(reg_addr);
    _wire->write((uint8_t)(data >> 8));
    _wire->write((uint8_t)(data & 0xFF));
    _wire->endTransmission();
}

// ============================================================================
// Private
// ============================================================================

void TLA20XX::writeConfig()
{
    // Keep the non-functional [4:0] bits at their datasheet default.
    _config = (_config & ~TLA20XX_CONFIG_RESERVED) | TLA20XX_CONFIG_RESERVED;
    writeRegister(TLA20XX_REG_CONFIG, _config);
}
