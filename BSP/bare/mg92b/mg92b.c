//
// Created by hanjin on 24-9-22.
//

#include "mg92b.h"

mg92b_data_t mg92b_data[18] = {
    /* LA */
    {0, &htim3, TIM_CHANNEL_1, 529, 1858, 90},
    {1, &htim3, TIM_CHANNEL_2, 439, 1868, 90},
    {1, &htim5, TIM_CHANNEL_4, 569, 1896, 90},
    /* LB */
    {0, &htim5, TIM_CHANNEL_1, 525, 1869, 90},
    {1, &htim5, TIM_CHANNEL_2, 423, 1928, 90},
    {1, &htim5, TIM_CHANNEL_3, 423, 1844, 90},
    /* LC */
    {0, &htim2, TIM_CHANNEL_2, 569, 1895, 90},
    {1, &htim9, TIM_CHANNEL_1, 480, 1870, 90},
    {1, &htim9, TIM_CHANNEL_2, 551, 1859, 90},

    /* RA */
    {1, &htim2, TIM_CHANNEL_4, 475, 1875, 90},
    {0, &htim2, TIM_CHANNEL_3, 539, 1868, 90},
    {0, &htim4, TIM_CHANNEL_1, 446, 1881, 90},
    /* RB */
    {1, &htim4, TIM_CHANNEL_2, 434, 1859, 90},
    {0, &htim4, TIM_CHANNEL_3, 471, 1819, 90},
    {0, &htim4, TIM_CHANNEL_4, 475, 1861, 90},
    /* RC */
    {1, &htim3, TIM_CHANNEL_4, 513, 1833, 90},
    {0, &htim2, TIM_CHANNEL_1, 467, 1870, 90},
    {0, &htim3, TIM_CHANNEL_3, 525, 1864, 90},
};

uint16_t __Get_CCR(mg92b_data_t* data)
{
    if (data->use_supp) // 是否使用补角
        return (uint16_t)(data->ccr_start + data->ccr_range - data->angle / 180 * data->ccr_range + 0.5);
    return (uint16_t)(data->ccr_start + data->angle / 180 * data->ccr_range + 0.5); // 四舍五入
}


/**
 * @brief 初始化全体舵机
 * @return 0 初始化成功 1~18 对应舵机PWM通道启动失败
 */
uint8_t MG92B_Init(void)
{
    // 启动PWM输出，并设置默认角度
    for (uint8_t i = 0; i < 18; i++)
    {
        HAL_StatusTypeDef ret = HAL_TIM_PWM_Start(mg92b_data[i].htim, mg92b_data[i].channel);
        __HAL_TIM_SET_COMPARE(mg92b_data[i].htim, mg92b_data[i].channel, __Get_CCR(&mg92b_data[i]));
        if (ret != HAL_OK) return i + 1;
    }
    return 0;
}

/**
 * @brief 设置舵机角度
 * @param data 舵机数据指针
 * @param angle 需要设置的角度 0~180
 * @return 0 设置成功 1 角度超出范围
 */
uint8_t MG92B_Set_Angle(mg92b_data_t* data, double angle)
{
    if (angle < 0 || angle > 180) return 1;
    data->angle = angle;
    __HAL_TIM_SET_COMPARE(data->htim, data->channel, __Get_CCR(data));
    return 0;
}

/**
 * @brief 设置所有舵机角度
 * @param angle 0~180
 * @return 0 设置成功 1 角度超出范围
 */
uint8_t MG92B_Set_All_Angle(double angle)
{
    if (angle < 0 || angle > 180) return 1;
    for (uint8_t i = 0; i < 18; i++)
        MG92B_Set_Angle(mg92b_data + i, angle);
    return 0;
}

/**
 * @brief 按照传入的数组设置舵机角度
 * @param angles 舵机角度数组
 * @param st 起始舵机编号
 * @param ed 终止舵机编号
 * @return
 */
uint8_t MG92B_Set_Angles(double angles[], const uint8_t st, const uint8_t ed)
{
    if (ed >= 18) return 1;
    for (uint8_t i = 0; i <= ed - st; i++)
        if (MG92B_Set_Angle(mg92b_data + i + st, angles[i]))
            return 0x10 | (i + 1);
    return 0;
}
