//
// Created by hanjin on 24-9-5.
//

#include "mx02s.h"

#include <stdio.h>

#include "pca9685.h"

uint8_t MX02S_Read_Buffer[128];
MX02S_StatusTypeDef MX02S_Status = MX02S_INITIALIZING;
MX02S_W_StatusTypeDef MX02S_W_Status = MX02S_W_NONE;
MX02S_S_StatusTypeDef MX02S_S_Status = MX02S_S_NONE;


uint8_t MX02S_aSend_Data(char* data)
{
    if (MX02S_S_Status == MX02S_S_SENDING) return 2;
    const HAL_StatusTypeDef ret = HAL_UART_Transmit_IT(&huart3, (uint8_t*)data, strlen(data));
    if (ret == HAL_OK)
    {
        MX02S_S_Status = MX02S_S_SENDING;
        return 0;
    } else
    {
        MX02S_S_Status = MX02S_S_ERROR;
        return 1;
    }
}

uint8_t MX02S_Send_Data(char* data)
{
    const HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart3, (uint8_t*)data, strlen(data), 1000);
    if (ret == HAL_OK) return 0;
    else return 1;
}

uint8_t MX02S_Send_CMD(char* cmd)
{
    // uint8_t sendBuffer[128];
    // sprintf(sendBuffer, "AT+%s\r\n", cmd);
    // const HAL_StatusTypeDef res = HAL_UART_Transmit(&huart3, sendBuffer, strlen(sendBuffer), 1000);
    // 嗯，，，节约性能？？？
    const HAL_StatusTypeDef ret = HAL_UART_Transmit(&huart3, (uint8_t*)cmd, strlen(cmd), 1000);
    if (ret == HAL_OK) return 0;
    else return 1;
}


/**
 * @brief 向蓝牙模块发送AT指令
 * @param cmd 指令 以AT+开头 以\r\n结尾
 * @return 0 成功 1 发送失败 2 其他指令正在等待回复，指令发送已取消 3 指令返回ERROR
 */
uint8_t MX02S_Send_W_CMD(char* cmd)
{
    if (MX02S_W_Status == MX02S_W_WAITING) return 2; // 其他指令等待回复中
    MX02S_W_Status = MX02S_W_WAITING;
    const uint8_t ret = MX02S_Send_CMD(cmd);
    if (ret) return ret;
    // 等待指令返回
    while (MX02S_W_Status == MX02S_W_WAITING);
    // HAL_UART_Transmit(&huart1, "2", 1, 100);
    if (MX02S_W_Status == MX02S_W_ERROR) return 3;
    MX02S_W_Status = MX02S_W_NONE;
    return 0;
}

uint8_t MX02S_Init(void)
{
    // 开始接收数据
    HAL_UARTEx_ReceiveToIdle_IT(&huart3, MX02S_Read_Buffer, sizeof(MX02S_Read_Buffer));
    // 执行初始化
    uint8_t ret;
    MX02S_Status = MX02S_INITIALIZING;
    ret = MX02S_Send_W_CMD("AT+REBOOT=1\r\n"); // 重启蓝牙模块
    if (ret) return 0x10 | ret;
    while (MX02S_Status != MX02S_READY); // 等待初始化
    ret = MX02S_Set_Name(MX02S_NAME);
    if (ret) return 0x20 | ret;
    ret = MX02S_Set_AMData(MX02S_AMDATA);
    if (ret) return 0x30 | ret;
    return 0;
}

uint8_t MX02S_Set_Name(char* name)
{
    char cmd[128];
    sprintf(cmd, "AT+NAME=%s\r\n", name);
    return MX02S_Send_W_CMD(cmd);
}

uint8_t MX02S_Set_AMData(char* amData)
{
    char cmd[128];
    sprintf(cmd, "AT+AMDATA=%s\r\n", amData);
    return MX02S_Send_W_CMD(cmd);
}

uint8_t MX02S_Set_Adv_Status(const uint8_t status)
{
    if (status > 1) return 4;
    char cmd[128];
    sprintf(cmd, "AT+ADV=%1d\r\n", status);
    return MX02S_Send_W_CMD(cmd);
}

void MX02S_W_Success_Handler(uint16_t Size)
{
    // HAL_UART_Transmit(&huart1, MX02S_Read_Buffer, Size, 1000);
}

void MX02S_W_Error_Handler(const uint16_t Size)
{
    HAL_UART_Transmit(&huart1, MX02S_Read_Buffer, Size, 1000);
}

void MX02S_Receive_Data(const uint16_t Size)
{
    // HAL_UART_Transmit(&huart1, MX02S_Read_Buffer, Size, 1000);
    if (MX02S_Read_Buffer[0] == '+')
    {
        // 读取响应
        if (Size == 8 &&
            MX02S_Read_Buffer[1] == 'R' &&
            MX02S_Read_Buffer[2] == 'E' &&
            MX02S_Read_Buffer[3] == 'A' &&
            MX02S_Read_Buffer[4] == 'D' &&
            MX02S_Read_Buffer[5] == 'Y'
        )
        {
            MX02S_Status = MX02S_READY;
        }
    } else if (MX02S_Read_Buffer[0] == 0xFF)
    {
        switch (MX02S_Read_Buffer[1])
        {
        case 0x00: // 重启指令
            // HAL_UART_Transmit(&huart1, "REBOOT", 6, 1000);
            NVIC_SystemReset();
        case 0x01: // 蜂鸣器响起
            // HAL_UART_Transmit(&huart1, "RING", 4, 1000);
            Ring_Counter = 200;
            break;
        case 0x02: // 测试指令 所有舵机角度增加 1
            // HAL_UART_Transmit(&huart1, "CMD3", 4, 1000);
            PCA9685_aSet_All_Angle(angles[0][0] + 1);
            break;
        case 0x03: // 测试指令 所有舵机角度减少 0.1
            // HAL_UART_Transmit(&huart1, "CMD4", 4, 1000);
            PCA9685_aSet_All_Angle(angles[0][0] - 1);
            break;
        case 0x04:
            HAL_UART_Transmit(&huart1, "CMD5", 4, 1000);
            break;
        default: ;
        }
    } else
    {
        if (MX02S_W_Status == MX02S_W_WAITING)
        {
            if (MX02S_Read_Buffer[0] == 'O')
            {
                MX02S_W_Status = MX02S_W_SUCCESS;
                MX02S_W_Success_Handler(Size);
            } else
            {
                MX02S_W_Status = MX02S_W_ERROR;
                MX02S_W_Error_Handler(Size);
            }
            MX02S_W_Status = MX02S_W_NONE;
        }
    }
}

void MX02S_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size)
{
    if (huart == &huart3)
    {
        MX02S_Receive_Data(Size);
        // 继续接收下一次数据，没有人知道为什么用dma就只能接收到一次
        // uint8_t res =
        HAL_UARTEx_ReceiveToIdle_IT(&huart3, MX02S_Read_Buffer, sizeof(MX02S_Read_Buffer));
        // HAL_UART_Transmit(&huart1, &res, 1, 1000);
    }
}


void MX02S_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
    if (huart == &huart3)
    {
        if (MX02S_S_Status == MX02S_S_SENDING)
        {
            MX02S_S_Status = MX02S_S_SUCCESS;
        }
    }
}
