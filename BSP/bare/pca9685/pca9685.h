//
// Created by hanjin on 24-9-3.
//

#ifndef PCA9685_H
#define PCA9685_H
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

#define __PCA9685_VERSION__ "0.0.1"

#define PCA9685_HI2C            &hi2c1

#define PCA9685_ALL             0x70
#define PCA9685_0               0x41 // A0 连接 1 0 0 0 0 0 1
#define PCA9685_1               0x42 // A1 连接 1 0 0 0 0 1 0

#define PCA9685_ANGLE_SYMMETRICAL 0  // 设置角度是否对称模式，0: Angle 0 ~ 180; 1 Angle -90 ~ 90

#define PCA9685_MODE1           0x00
#define PCA9685_MODE2           0x01
#define PCA9685_SUBADR1         0x02
#define PCA9685_SUBADR2         0x03
#define PCA9685_SUBADR3         0x04
#define PCA9685_ALLCALL         0x05

// 0 <= X <= 15
#define PCA9685_LEDX_ON_L(X)    0x06 + (X) * 4
#define PCA9685_LEDX_ON_H(X)    0x07 + (X) * 4
#define PCA9685_LEDX_OFF_L(X)   0x08 + (X) * 4
#define PCA9685_LEDX_OFF_H(X)   0x09 + (X) * 4

#define PCA9685_ALL_LED_ON_L    0xFA
#define PCA9685_ALL_LED_ON_H    0xFB
#define PCA9685_ALL_LED_OFF_L   0xFC
#define PCA9685_ALL_LED_OFF_H   0xFD

#define PCA9685_PRE_SCALE       0xFE
#define PCA9685_TEST_MODE       0xFF

typedef struct
{
    double off, range;
} PCA9685_Led_Info;

extern double angles[2][16];

extern const PCA9685_Led_Info PCA9685_Led_Off_Range[2][16];

extern uint8_t PCA9685_Send_Ready;

uint8_t PCA9685_Read_Byte(const uint8_t slave, const uint8_t reg);

uint8_t PCA9685_Init();
uint8_t PCA9685_Set_Freq(const uint8_t slave, double freq);
uint8_t PCA9685_Set_Angle(const uint8_t slave, const uint8_t led, const double angle);
uint8_t PCA9685_Set_Angles(const uint8_t slave_id, const double angles[16]);
uint8_t PCA9685_Set_Angles_Range(const uint8_t slave_id, const double angles[], const uint8_t st, const uint8_t ed);
uint8_t PCA9685_Set_All_Angle(const double angle);
uint8_t PCA9685_aSet_All_Angle(const double angle);

#endif //PCA9685_H
