//
// Created by hanjin on 24-9-1.
//

#include "mpu6050.h"

void i2c_write(unsigned char slave_addr, unsigned char reg_addr,
               unsigned char length, unsigned char* data) {
 HAL_I2C_Mem_Write(&hi2c2, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 1000);
}

void i2c_read(unsigned char slave_addr, unsigned char reg_addr,
              unsigned char length, unsigned char* data) {
 HAL_I2C_Mem_Read(&hi2c2, slave_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, length, 1000);
}

void delay_ms(unsigned long num_ms) {
 HAL_Delay(num_ms);
}

void get_ms(unsigned long* count) {
 *count = HAL_GetTick();
}


/**
 * @brief IIC读字节
 * @param reg 寄存器地址
 */
uint8_t MPU_Read_Byte(const uint8_t reg) {
 uint8_t res;
 HAL_I2C_Mem_Read(&hi2c2, MPU_READ_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, 0xFFFF);
 return res;
}

uint8_t* MPU_Read_Len(const uint8_t st, const uint8_t len) {
 if (len > 16) return NULL;
 static uint8_t res[16];
 HAL_I2C_Mem_Read(&hi2c2, MPU_READ_ADDR, st, I2C_MEMADD_SIZE_8BIT, res, len, 0xFFFF);
 return res;
}

uint8_t MPU_Write_Byte(const uint8_t reg, uint8_t data) {
 const uint8_t ret = HAL_I2C_Mem_Write(&hi2c2, MPU_WRITE_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 0xFFFF);
 if (ret == HAL_OK) {
  return 1;
 } else {
  return 0;
 }
}


void MPU_Init(void) {
 MPU_Write_Byte(MPU_PWR1_MEM_ADDR, 0x80); // 复位
 HAL_Delay(100);
 MPU_Write_Byte(MPU_PWR1_MEM_ADDR, 0x00); // 启动

 MPU_Set_Gyro_Range(3);
 MPU_Set_Accel_Range(0);
 MPU_Set_Freq(100);

 MPU_Write_Byte(MPU_INT_ADDR, 0x00);
 MPU_Write_Byte(MPU_USER_ADDR, 0x00);
 MPU_Write_Byte(MPU_FIFO_EN_ADDR, 0x00);
 MPU_Write_Byte(MPU_INT_PIN_ADDR, 0x00);

 MPU_Write_Byte(MPU_PWR1_MEM_ADDR, 0x01);
 MPU_Write_Byte(MPU_PWR2_MEM_ADDR, 0x00);
}

/**
 * @brief 设置陀螺仪范围，建议 3
 */
void MPU_Set_Gyro_Range(const uint8_t FS_SEL) {
 if (FS_SEL > 3 || FS_SEL < 0) return;
 MPU_Write_Byte(MPU_GYRO_ADDR, FS_SEL << 3);
}

/**
 * @brief 设置加速度传感器范围，建议 0
 */
void MPU_Set_Accel_Range(const uint8_t AFS_SEL) {
 if (AFS_SEL > 3 || AFS_SEL < 0) return;
 MPU_Write_Byte(MPU_ACCEL_ADDR, AFS_SEL << 3);
}

void MPU_Set_Freq(int freq) {
 uint8_t SMPLRT_DIV;
 if (freq > 8000) freq = 8000;
 else if (freq < 4) freq = 4;
 if (freq >= 256 * 2) {
  SMPLRT_DIV = 8000 / freq - 1;
 } else {
  SMPLRT_DIV = 1000 / freq - 1;
 }
 MPU_Write_Byte(MPU_OUT_FRE_DIV_ADDR, SMPLRT_DIV);
 // 默认将LPF设为Rate的一半
 MPU_Set_LPF(freq / 2);
}

void MPU_Set_LPF(const int lpf) {
 uint8_t data;
 if (lpf >= 256) data = 0;
 else if (lpf >= 188) data = 1;
 else if (lpf >= 98) data = 2;
 else if (lpf >= 42) data = 3;
 else if (lpf >= 20) data = 4;
 else if (lpf >= 10) data = 5;
 else data = 6;
 MPU_Write_Byte(MPU_DLPF_ADDR, data);
}

uint8_t MPU_Get_ID() {
 return MPU_Read_Byte(MPU_ID_ADDR);
}

/**
 * @brief 获取陀螺仪数据
 */
void MPU_Get_Gyro(short data[]) {
 // data[0] = ((uint16_t)MPU_Read_Byte(MPU_GYRO_XOUT_H) << 8) | MPU_Read_Byte(MPU_GYRO_XOUT_L);
 // data[1] = ((uint16_t)MPU_Read_Byte(MPU_GYRO_YOUT_H) << 8) | MPU_Read_Byte(MPU_GYRO_YOUT_L);
 // data[2] = ((uint16_t)MPU_Read_Byte(MPU_GYRO_ZOUT_H) << 8) | MPU_Read_Byte(MPU_GYRO_ZOUT_L);
 const uint8_t* buf = MPU_Read_Len(MPU_GYRO_XOUT_H, 6);
 data[0] = ((uint16_t)buf[0] << 8) | buf[1];
 data[1] = ((uint16_t)buf[2] << 8) | buf[3];
 data[2] = ((uint16_t)buf[4] << 8) | buf[5];
}

/**
 * @brief 获取加速度传感器数据
 */
void MPU_Get_Accel(short data[]) {
 // data[0] = ((uint16_t)MPU_Read_Byte(MPU_ACCEL_XOUT_H) << 8) | MPU_Read_Byte(MPU_ACCEL_XOUT_L);
 // data[1] = ((uint16_t)MPU_Read_Byte(MPU_ACCEL_YOUT_H) << 8) | MPU_Read_Byte(MPU_ACCEL_YOUT_L);
 // data[2] = ((uint16_t)MPU_Read_Byte(MPU_ACCEL_ZOUT_H) << 8) | MPU_Read_Byte(MPU_ACCEL_ZOUT_L);
 const uint8_t* buf = MPU_Read_Len(MPU_ACCEL_XOUT_H, 6);
 data[0] = ((uint16_t)buf[0] << 8) | buf[1];
 data[1] = ((uint16_t)buf[2] << 8) | buf[3];
 data[2] = ((uint16_t)buf[4] << 8) | buf[5];
}

/**
 * @brief 获取陀螺仪和加速度传感器数据
 */
void MPU_Get_Data(short Gyro[], short Accel[]) {
 MPU_Get_Gyro(Gyro);
 MPU_Get_Accel(Accel);
}
