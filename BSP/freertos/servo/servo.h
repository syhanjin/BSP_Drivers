//
// Created by hanjin on 24-9-22.
//

#ifndef SERVO_H
#define SERVO_H

#include "tim.h"
#include <stdio.h>

#define __SERVO_VERSION__ "0.0.2"

typedef struct
{
    const uint8_t use_supp;        ///< 是否使用补角
    TIM_HandleTypeDef *const htim; ///< PWM 使用的定时器
    const uint32_t channel;        ///< PWM使用的定时器通道
#if SERVO_DEBUG == 0
    const uint32_t ccr_start, ccr_range; ///< ccr 起始和范围
#else
    uint32_t ccr_start, ccr_range;
#endif
    float angle; ///< 当前角度
} Servo_t;

uint8_t SERVO_Init(const Servo_t *servo);
void SERVO_Stop(const Servo_t *servo);
uint8_t SERVO_Set_Angle(Servo_t *servo, float angle);
#endif // SERVO_H
