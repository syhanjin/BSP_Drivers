//
// Created by hanjin on 24-9-1.
//

#ifndef MPU6050_H
#define MPU6050_H

#define __MPU6050_VERSION__ "0.0.1"

#define MPU6050
#define EMPL_TARGET_MSP430
#include "i2c.h"
#include "mpu6050_reg.h"
void i2c_write(unsigned char slave_addr, unsigned char reg_addr,
               unsigned char length, unsigned char const* data);
void i2c_read(unsigned char slave_addr, unsigned char reg_addr,
              unsigned char length, unsigned char* data);
void delay_ms(unsigned long num_ms);
void get_ms(unsigned long* count);

#include "inv_mpu_dmp_motion_driver.h"

void MPU_Init(void);

void MPU_Set_Gyro_Range(const uint8_t FS_SEL);
void MPU_Set_Accel_Range(const uint8_t AFS_SEL);

void MPU_Set_Freq(int freq);
void MPU_Set_LPF(const int lpf);

uint8_t MPU_Get_ID();
void MPU_Get_Gyro(short* data);
void MPU_Get_Accel(short* data);
void MPU_Get_Data(short* Gyro, short* Accel);

#endif //MPU6050_H
