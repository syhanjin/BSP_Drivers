//
// Created by hanjin on 24-9-4.
//

#include "jy901s.h"


JY901S_StatusTypeDef JY901S_Status = JY901S_RESET;
JY901S_DataTypeDef JY901S_Data;
uint8_t JY901S_Read_Buffer[11];

uint8_t JY901S_Write(const uint8_t reg, const uint16_t data) {
    HAL_StatusTypeDef res;
    const uint8_t sendBuffer[5] = {
        0xFF, 0xAA, reg, data & 0xFF, data >> 8
    };
    res = HAL_UART_Transmit(&huart2, sendBuffer, 5, 1000);
    if (res == HAL_OK) return 0;
    else return 1;
}

/**
 * @brief 解锁传感器，时效为10s
 * @return
 */
uint8_t JY901S_Unlock() {
    const uint8_t res = JY901S_Write(JY901S_KEY, 0xB588);
    HAL_Delay(200);
    return res;
}

uint8_t JY901S_Save() {
    return JY901S_Write(JY901S_SAVE, 0x0000);
}

/**
 * @brief 发送一条指令
 * @param reg 寄存器
 * @param data 数据
 * @return
 */
uint8_t JY901S_Send_CMD(const uint8_t reg, const uint16_t data) {
    uint8_t res = JY901S_Unlock();
    res |= JY901S_Write(reg, data);
    res |= JY901S_Save();
    return res;
}

/**
 * @brief 初始化JY901S
 * @return
 */
uint8_t JY901S_Init(void) {
    uint8_t res;
    // 重启传感器
    res = JY901S_Write(JY901S_SAVE, 0x00FF);
    HAL_Delay(200);
    if (res) return 1;
    JY901S_Status = JY901S_RECEIVING;
    HAL_UART_Receive_DMA(&huart2, JY901S_Read_Buffer, 11);
    return 0;
}

/**
 * @brief 处理ReadBuffer中的数据，并保存到Data里
 * @attention 目前支持的处理：时间，加速度，温度，角速度，角度，版本，磁场，温度，气压，高度，四元数
 * @return 0 OK; 1 校验和未通过; 2 不是接收数据返回值
 */
uint8_t JY901S_GetData() {
    // 计算校验和
    uint8_t sum = 0;
    for (uint8_t i = 0; i < 10; i++) sum += JY901S_Read_Buffer[i];
    if (sum != JY901S_Read_Buffer[10]) {
        // 使错误LED灯亮起 200 ms
        MData_Error_Counter = 200;
        HAL_UART_Receive_DMA(&huart2, JY901S_Read_Buffer, 11);
        return 1;
    }
    if (JY901S_Read_Buffer[0] != 0x55) return 2;
    switch (JY901S_Read_Buffer[1]) {
    case 0x50: // 时间
        JY901S_Data.year = JY901S_Read_Buffer[2];
        JY901S_Data.month = JY901S_Read_Buffer[3];
        JY901S_Data.day = JY901S_Read_Buffer[4];
        JY901S_Data.hour = JY901S_Read_Buffer[5];
        JY901S_Data.minute = JY901S_Read_Buffer[6];
        JY901S_Data.second = JY901S_Read_Buffer[7];
        JY901S_Data.millisecond = ((uint16_t)JY901S_Read_Buffer[9] << 8) | JY901S_Read_Buffer[8];
        break;
    case 0x51: // 加速度
        JY901S_Data.ax = (double)(((short)JY901S_Read_Buffer[3] << 8) | JY901S_Read_Buffer[2]) / 32768 * 16 * g;
        JY901S_Data.ay = (double)(((short)JY901S_Read_Buffer[5] << 8) | JY901S_Read_Buffer[4]) / 32768 * 16 * g;
        JY901S_Data.az = (double)(((short)JY901S_Read_Buffer[7] << 8) | JY901S_Read_Buffer[6]) / 32768 * 16 * g;
        JY901S_Data.temperature = (double)(((short)JY901S_Read_Buffer[9] << 8) | JY901S_Read_Buffer[8]) / 100;
        break;
    case 0x52: // 角速度
        JY901S_Data.gx = (double)(((short)JY901S_Read_Buffer[3] << 8) | JY901S_Read_Buffer[2]) / 32768 * 2000;
        JY901S_Data.gy = (double)(((short)JY901S_Read_Buffer[5] << 8) | JY901S_Read_Buffer[4]) / 32768 * 2000;
        JY901S_Data.gz = (double)(((short)JY901S_Read_Buffer[7] << 8) | JY901S_Read_Buffer[6]) / 32768 * 2000;
        break;
    case 0x53: // 角度
        JY901S_Data.roll = (double)(((short)JY901S_Read_Buffer[3] << 8) | JY901S_Read_Buffer[2]) / 32768 * 180;
        JY901S_Data.pitch = (double)(((short)JY901S_Read_Buffer[5] << 8) | JY901S_Read_Buffer[4]) / 32768 * 180;
        JY901S_Data.yaw = (double)(((uint16_t)JY901S_Read_Buffer[7] << 8) | JY901S_Read_Buffer[6]) / 32768 * 180;
        JY901S_Data.version = ((uint16_t)JY901S_Read_Buffer[9] << 8) | JY901S_Read_Buffer[8];
        break;
    case 0x54: // 磁场
        JY901S_Data.hx = (double)(((short)JY901S_Read_Buffer[3] << 8) | JY901S_Read_Buffer[2]) / 32768 / 150;
        JY901S_Data.hy = (double)(((short)JY901S_Read_Buffer[5] << 8) | JY901S_Read_Buffer[4]) / 32768 / 150;
        JY901S_Data.hz = (double)(((short)JY901S_Read_Buffer[7] << 8) | JY901S_Read_Buffer[6]) / 32768 / 150;
        JY901S_Data.temperature = (double)((uint16_t)JY901S_Read_Buffer[9] << 8 | JY901S_Read_Buffer[8]) / 100;
        break;
    case 0x55: // 端口状态输出
        break;
    case 0x56: // 气压高度输出
        JY901S_Data.pressure =
            (int)JY901S_Read_Buffer[5] << 24 | JY901S_Read_Buffer[4] << 16 | JY901S_Read_Buffer[3] << 8 | JY901S_Read_Buffer[2];
        JY901S_Data.height =
            (int)JY901S_Read_Buffer[9] << 24 | JY901S_Read_Buffer[8] << 16 | JY901S_Read_Buffer[7] << 8 | JY901S_Read_Buffer[6];
        break;
    case 0x57:
        break;
    case 0x58:
        break;
    case 0x59: // 四元数
        JY901S_Data.q0 = (double)(((short)JY901S_Read_Buffer[3] << 8) | JY901S_Read_Buffer[2]) / 32768;
        JY901S_Data.q1 = (double)(((short)JY901S_Read_Buffer[5] << 8) | JY901S_Read_Buffer[4]) / 32768;
        JY901S_Data.q2 = (double)(((short)JY901S_Read_Buffer[7] << 8) | JY901S_Read_Buffer[6]) / 32768;
        JY901S_Data.q3 = (double)(((short)JY901S_Read_Buffer[9] << 8) | JY901S_Read_Buffer[8]) / 32768;
        break;
    case 0x5A:
        break;
    default:
        return 3;
    }
    HAL_UART_Receive_DMA(&huart2, JY901S_Read_Buffer, 11);

    return 0;
}

/**
 * @brief DMA接收数据完成事件
 * @param huart
 */
void JY901S_UART_RxCpltCallback(UART_HandleTypeDef* huart) {
    if (huart == &huart2) {
        if (JY901S_Status == JY901S_RECEIVING) {
            JY901S_GetData();
        }
    }
}
