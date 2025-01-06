//
// Created by hanjin on 24-9-1.
//

#ifndef MPU6050_REG_H
#define MPU6050_REG_H

#define MPU_ADDR                     0xD0
#define MPU_READ_ADDR                0xD1
#define MPU_WRITE_ADDR               0xD0

#define MPU_OUT_FRE_DIV_ADDR         0x19        //采样频率地址   默认8Khz  如果开了低通滤波器 则变为1Khz
#define MPU_DLPF_ADDR                0x1A		 //低通滤波器地址

#define MPU_GYRO_ADDR			     0x1B        // 陀螺仪配置
#define MPU_ACCEL_ADDR               0x1C        // 加速度传感器配置地址


#define MPU_ID_ADDR                  0x75        //MPU ID的地址
#define MPU_INT_ADDR				 0X38		 //mpu中断使能地址
#define MPU_USER_ADDR				 0x6A        //MPU用户控制地址  用于控制FIFO  I2C主模式
#define MPU_FIFO_EN_ADDR			 0x23		 //MPU的FIFO使能地址
#define MPU_INT_PIN_ADDR			 0x37        //INT中断地址
#define MPU_PWR1_MEM_ADDR            0x6B		 //电源管理1的地址
#define MPU_PWR2_MEM_ADDR            0x6C		 //电源管理2的地址


//加速度计 数据寄存器地址
#define MPU_ACCEL_XOUT_H				  0x3B
#define MPU_ACCEL_XOUT_L				  0x3C
#define MPU_ACCEL_YOUT_H				  0x3D
#define MPU_ACCEL_YOUT_L  				  0x3E
#define MPU_ACCEL_ZOUT_H				  0x3F
#define MPU_ACCEL_ZOUT_L				  0x40

//陀螺仪  数据寄存器地址
#define MPU_GYRO_XOUT_H					  0x43
#define MPU_GYRO_XOUT_L					  0x44
#define MPU_GYRO_YOUT_H				      0x45
#define MPU_GYRO_YOUT_L					  0x46
#define MPU_GYRO_ZOUT_H					  0x47
#define MPU_GYRO_ZOUT_L					  0x48
#endif //MPU6050_REG_H
