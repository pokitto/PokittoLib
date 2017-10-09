/*
 * mbed Library to use a software master i2c interface on any GPIO pins
 * Copyright (c) 2012 Christopher Pepper
 * Released under the MIT License: http://mbed.org/license/mit
 */

#include "SoftwareI2C.h"

/**
 * @brief Initializes interface
 * @param sda GPIO pin to use as I2C SDA
 * @param scl GPIO pin to use as I2C SCL
 */

SoftwareI2C::SoftwareI2C(PinName sda, PinName scl) : _sda(sda) , _scl(scl) {
    _scl.output();
    _scl.mode(OpenDrain);
    _sda.output();
    _sda.mode(OpenDrain);

    _device_address = 0;
    _frequency_delay = 1;

    initialise();
}

SoftwareI2C::~SoftwareI2C() {

}

/**
 * @brief Read 1 or more bytes from the I2C slave
 * @param device_address The address of the device to read from
 * @param data An allocated array to read the data into
 * @param data_bytes Number of bytes to read (must be equal to or less then the allocated memory in data)
 */
void SoftwareI2C::read(uint8_t device_address, uint8_t* data, uint8_t data_bytes) {
    if (data == 0 || data_bytes == 0) return;

    device_address = device_address | 0x01;
    start();
    putByte(device_address);
    getAck();
    for (int x = 0; x < data_bytes; ++x) {
        data[x] = getByte();
        if ( x < (data_bytes -1)) { //ack all but the final byte
            giveAck();
        }
    }
    stop();
}

/**
 * @brief Write 1 or more bytes to the I2C slave
 * @param device_address The address of the device to write to
 * @param data An array to write the data from
 * @param data_bytes Number of bytes to write from array
 */
void SoftwareI2C::write(uint8_t device_address, uint8_t* data,  uint8_t data_bytes) {
    if (data == 0 || data_bytes == 0) return;

    device_address = device_address & 0xFE;
    start();
    putByte(device_address);
    getAck();
    for ( int x = 0; x < data_bytes; ++x ) {
        putByte(data[x]);
        getAck();
    }
    stop();
}

/**
 * @brief Write 1 byte to the I2C slave
 * @param device_address The address of the device to write to
 * @param byte The data to write
 */
void SoftwareI2C::write(uint8_t device_address, uint8_t byte) {
    device_address = device_address & 0xFE;
    start();
    putByte(device_address);
    getAck();
    putByte(byte);
    getAck();
    stop();
}

/**
 * @brief Read 1 or more bytes from the I2C slave at the specified memory address
 * @param device_address The address of the device to read from
 * @param start_address The memory address to read from
 * @param data The allocated array to read into
 * @param data_bytes The number of bytes to read
 */
void SoftwareI2C::randomRead(uint8_t device_address, uint8_t start_address, uint8_t* data, uint8_t data_bytes) {
    if (data == 0 || data_bytes == 0) return;

    device_address = device_address & 0xFE;
    start();
    putByte(device_address);
    if (!getAck()) {
        return;
    }
    putByte(start_address);
    if (!getAck()) {
        return;
    }

    device_address=device_address | 0x01;
    start();
    putByte(device_address);
    if (!getAck()) {
        return;
    }
    for ( int x = 0; x < data_bytes; ++x) {
        data[x] = getByte();
        if (x != (data_bytes - 1)) giveAck();
    }
    stop();
}

/**
 * @brief Write 1 byte to the I2C slave at the specified memory address
 * @param device_address The address of the device to write to
 * @param start_address The memory address to write to
 * @param byte The data to write
 */
void SoftwareI2C::randomWrite(uint8_t device_address, uint8_t start_address, uint8_t byte) {
    device_address = device_address & 0xFE;
    start();
    putByte(device_address);
    getAck();
    putByte(start_address);
    getAck();
    putByte(byte);
    getAck();
    stop();
}

/**
 * @brief Write 1 or more bytes to the I2C slave at the specified memory address
 * @param device_address The address of the device to write to
 * @param start_address The memory address to write to
 * @param data The data to write
 * @param data_bytes The number of bytes to write
 */
void SoftwareI2C::randomWrite(uint8_t device_address, uint8_t start_address, uint8_t* data, uint8_t data_bytes) {
    if (data == 0 || data_bytes == 0) return;

    device_address = device_address & 0xFE;
    start();
    putByte(device_address);
    getAck();
    putByte(start_address);
    getAck();
    for ( int x = 0; x <= data_bytes; ++x ) {
        putByte(data[x]);
        getAck();
    }
    stop();
}

/**
 * @brief Read 2 bytes from the I2C slave at the specified memory address and return them as an 16bit unsigned integer
 * @param device_address The address of the device to read from
 * @param start_address The memory address to read from
 * @return MSB 16bit unsigned integer
 */
uint16_t SoftwareI2C::read16(uint8_t device_address, uint8_t start_address) {
    uint8_t short_array[2] = {0, 0};
    randomRead(device_address, start_address, short_array, 2 );
    uint16_t value = 0;
    value = short_array[0] << 8;
    value |= short_array[1];

    return value;
}

/**
 * @brief Read 3 bytes from the I2C slave at the specified memory address and return them as an 32bit unsigned integer
 * @param device_address The address of the device to read from
 * @param start_address The memory address to read from
 * @return MSB 32bit unsigned integer
 */
uint32_t SoftwareI2C::read24(uint8_t device_address, uint8_t start_address) {
    uint8_t value_array[4] = {0, 0, 0};
    randomRead(device_address, start_address, value_array, 3 );
    uint32_t value = 0;
    value = value_array[0] << 16;
    value |= value_array[1] << 8;
    value |= value_array[2];

    return value;
}

/**
 * @brief Read 4 bytes from the I2C slave at the specified memory address and return them as an 32bit unsigned integer
 * @param device_address The address of the device to read from
 * @param start_address The memory address to read from
 * @return MSB 32bit unsigned integer
 */
uint32_t SoftwareI2C::read32(uint8_t device_address, uint8_t start_address) {
    uint8_t value_array[4] = {0, 0, 0, 0};
    randomRead(device_address, start_address, value_array, 4 );
    uint32_t value = 0;
    value = value_array[0] << 24;
    value |= value_array[1] << 16;
    value |= value_array[2] << 8;
    value |= value_array[3];

    return value;
}

