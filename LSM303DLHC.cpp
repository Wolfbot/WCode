//=============================================================================
// Name        : LSM303DLHC.cpp
// Author      : -NA-
// Description : Class; defines the LSM303DLHC API
//=============================================================================

#include"LSM303DLHC.h"
#include<math.h>
#include<stdio.h>

/*Conection to Raspberry PI:
 LSM303     Raspberry PI
 VDD    ->  3V3(PIN 1)
 SDA    ->  SDA(PIN 3)
 SCL    ->  SCL(PIN 5)
 GND    ->  GND(PIN 6)
*/

#define LSM303DLHC_MAG_ADDRESS            (0x3C >> 1)
#define LSM303DLHC_ACC_ADDRESS            (0x32 >> 1)

LSM303::LSM303(const char * i2cDeviceName) : i2c_lsm303(i2cDeviceName)
{

}

uint8_t LSM303::readAccRegister(uint8_t regAddr)
{
    i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);
    return i2c_lsm303.readByte(regAddr);
}

uint8_t LSM303::readMagRegister(uint8_t regAddr)
{
    i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
    return i2c_lsm303.readByte(regAddr);
}

void LSM303::writeAccRegister(uint8_t regAddr,uint8_t byte)
{
    i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);
    i2c_lsm303.writeByte(regAddr, byte);

}

void LSM303::writeMagRegister(uint8_t regAddr, uint8_t byte)
{
    i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
    i2c_lsm303.writeByte(regAddr, byte);

}

void LSM303::enable(void)
{
   writeAccRegister(LSM303_CTRL_REG1, 0b00100111);
   writeAccRegister(LSM303_CTRL_REG4, 0b00001000);

   writeMagRegister(LSM303_MR_REG, 0x00);
}

void LSM303::readAccelerationRaw(void)
{
    uint8_t block[6];

    i2c_lsm303.addrSet(LSM303DLHC_ACC_ADDRESS);

    i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_L_A, sizeof(block), block);

    acc_x_raw = (int16_t)(block[0] | block[1] << 8) >> 4;
    acc_y_raw = (int16_t)(block[2] | block[3] << 8) >> 4;
    acc_z_raw = (int16_t)(block[4] | block[5] << 8) >> 4;

}

void LSM303::readMagnetometerRaw(void)
{
    uint8_t block[6];

    i2c_lsm303.addrSet(LSM303DLHC_MAG_ADDRESS);
    i2c_lsm303.readBlock(0x80 | LSM303_OUT_X_H_M, sizeof(block), block);

    mag_x_raw = (int16_t)(block[1] | block[0] << 8);
    mag_y_raw = (int16_t)(block[5] | block[4] << 8);
    mag_z_raw = (int16_t)(block[3] | block[2] << 8);

}

void LSM303::readAcceleration(void)
{
    readAccelerationRaw();
}



