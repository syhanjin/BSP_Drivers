//
// Created by hanjin on 24-9-22.
//

#include "servo.h"

uint16_t __Get_CCR(const Servo_t *servo)
{
    if (servo->use_supp) // 是否使用补角
        return (uint16_t)(servo->ccr_start + servo->ccr_range - servo->angle / 180 * servo->ccr_range + 0.5);
    return (uint16_t)(servo->ccr_start + servo->angle / 180 * servo->ccr_range + 0.5); // 四舍五入
}

/**
 * @brief 初始化舵机
 * @param servo 舵机
 * @return 0 成功 1 失败
 */
uint8_t SERVO_Init(const Servo_t *servo)
{
    // 启动PWM输出，并设置默认角度
    const HAL_StatusTypeDef ret = HAL_TIM_PWM_Start(servo->htim, servo->channel);
    __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, __Get_CCR(servo));
    if (ret == HAL_OK)
        return 0;
    else
        return 1;
}

/**
 * @brief 停止舵机输出
 * @param servo
 */
void SERVO_Stop(const Servo_t *servo)
{
    HAL_TIM_PWM_Stop(servo->htim, servo->channel);
    __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, 0);
}

/**
 * @brief 设置舵机角度
 * @param servo 舵机数据指针
 * @param angle 需要设置的角度 0~180
 * @return 0 设置成功 1 角度超出范围
 */
uint8_t SERVO_Set_Angle(Servo_t *servo, float angle)
{
    if (angle < 0 || angle > 180) return 1;
    servo->angle = angle;
    __HAL_TIM_SET_COMPARE(servo->htim, servo->channel, __Get_CCR(servo));
    return 0;
}
