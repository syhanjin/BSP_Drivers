//
// Created by hanjin on 24-9-22.
//

#ifndef MG92B_H
#define MG92B_H

#include "tim.h"
#include "conf.h"
#include <stdio.h>

#define __MG92B_VERSION__ "0.1.0"

typedef struct
{
    const uint8_t use_supp; ///< 是否使用补角
    TIM_HandleTypeDef* const htim;
    const uint32_t channel;
#if MG92B_DEBUG == 0
    const uint32_t ccr_start, ccr_range;
#else
    uint32_t ccr_start, ccr_range;
#endif
    double angle;
} mg92b_data_t;


extern mg92b_data_t mg92b_data[18];

uint8_t MG92B_Init(void);
uint8_t MG92B_Set_Angle(mg92b_data_t* data, double angle);
uint8_t MG92B_Set_All_Angle(double angle);
uint8_t MG92B_Set_Angles(double angles[], const uint8_t st, const uint8_t ed);
#endif //MG92B_H
