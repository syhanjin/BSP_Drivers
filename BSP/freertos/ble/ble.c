//
// Created by hanjin on 24-9-23.
//

#include "ble.h"

#include "cmsis_os2.h"
#include "projdefs.h"
#include "task.h"

#include <stdio.h>
#include <string.h>
#include "usart.h"

BLE_Rx_t BLE_rx_buffer;

uint8_t BLE_Send_CMD(char *cmd)
{
    const HAL_StatusTypeDef ret = HAL_UART_Transmit(BLE_UART_HANDLE, (uint8_t *)cmd, strlen(cmd), 1000);
    if (ret == HAL_OK)
        return 0;
    else
        return 1;
}

/**
 * @brief 向蓝牙模块发送写入AT指令
 * @param cmd 指令 以AT+开头 以\r\n结尾
 * @return 0 成功 1 发送失败 2 其他指令正在等待回复，指令发送已取消 3 指令返回ERROR 4 超时 5 未知错误
 */
uint8_t BLE_Send_W_CMD(char *cmd)
{
    osMutexAcquire(BLESendMutexHandle, osWaitForever);
    const uint8_t ret = BLE_Send_CMD(cmd);
    if (ret) return ret;

    // 初始状态是不可被获得的
    osStatus_t result = osSemaphoreAcquire(BLEWaitResultSemaphoreHandle, BLE_W_CMD_TIMEOUT);

    osMutexRelease(BLESendMutexHandle);
    if (result == osErrorTimeout) {
        return 4;
    }
    if (result != osOK) {
        return 5;
    }
    // 注意 该执行方案下指令返回错误不会被识别
    // if (BLE_W_Status == BLE_W_ERROR) return 3;
    return 0;
}

/**
 * @brief 初始化蓝牙模块
 * @return .
 *  - 0x1x 重启蓝牙模块失败
 *  - 0x2x 设置蓝牙名称失败
 *  - 0x3x 设置AMData失败
 *  x: 与BLE_Send_W_CMD返回值相同
 */
uint8_t BLE_Init(void)
{
    BLE_Wakeup();
    osDelay(10);
    // 执行初始化
    uint8_t ret;
    ret = BLE_Send_W_CMD("AT+REBOOT=1\r\n"); // 重启蓝牙模块
    if (ret) return 0x10 | ret;
    // printf("REBOOT OK\n");
    /* 等待初始化 */
    osEventFlagsWait(globalEventHandle, BLE_SIGNALS_RECEIVE | BLE_FLAGS_READY, osFlagsWaitAll | osFlagsNoClear, osWaitForever);
    ret = BLE_Set_Name(BLE_NAME);
    if (ret) return 0x20 | ret;
    // printf("NAME OK\n");
    ret = BLE_Set_AMData(BLE_AMDATA);
    if (ret) return 0x30 | ret;
    // printf("AMDATA OK\n");
    return 0;
}

uint8_t BLE_Set_Name(char *name)
{
    char cmd[128] = "AT+NAME=";
    uint8_t index = strlen(cmd);
    // NOTE: 注意不要使用 sprintf 以及 printf 等一切与 malloc 相关的函数，会直接把整个任务调度搞崩溃
    while (*name != '\0') cmd[index++] = *(name++);
    cmd[index++] = '\r';
    cmd[index]   = '\n';
    return BLE_Send_W_CMD(cmd);
}

uint8_t BLE_Set_AMData(char *amData)
{
    char cmd[128];
    uint8_t index = strlen(cmd);
    // NOTE: 注意不要使用 sprintf 以及 printf 等一切与 malloc 相关的函数，会直接把整个任务调度搞崩溃
    while (*amData != '\0') cmd[index++] = *(amData++);
    cmd[index++] = '\r';
    cmd[index]   = '\n';
    return BLE_Send_W_CMD(cmd);
}

/**
 * 让 BLE 模块进入休眠模式
 */
void BLE_Sleep(void)
{
    HAL_GPIO_WritePin(BLE_SLP_GPIO_Port, BLE_SLP_Pin, GPIO_PIN_SET);
}

/**
 * 让 BLE 进入工作模式
 */
void BLE_Wakeup(void)
{
    HAL_GPIO_WritePin(BLE_SLP_GPIO_Port, BLE_SLP_Pin, GPIO_PIN_RESET);
}

void BLE_Rx_Error_Handler(void)
{
}

/**
 * @brief 处理命令
 * @attention 不要在此函数内运行任何具有延时的功能
 * @param cmd
 * @param data
 */
__weak void CMD_Handler(uint8_t cmd, uint8_t data[])
{
}

/**
 * @brief 蓝牙串口接收任务，处理接收到的信息
 * @attention 上位机发送的指令以 BLE_CMD_BEGIN 开头，第二个字节为指令的类型 后面的内容可以是参数
 */
void StartReceiveBLEData(void *argument)
{
    /* USER CODE BEGIN StartReceiveBLEData */
    // 开始接收事件
    osEventFlagsSet(globalEventHandle, BLE_FLAGS_RECEIVE);
    /* Infinite loop */
    for (;;) {
        HAL_UARTEx_ReceiveToIdle_IT(BLE_UART_HANDLE, BLE_rx_buffer.buffer, BLE_RX_BUFFER_SIZE);
        // 等待接收
        osThreadFlagsWait(BLE_SIGNALS_RECEIVE, osFlagsWaitAll, osWaitForever);

        // 需要限制字符串长度，否则会导致后面strcmp出错
        BLE_rx_buffer.buffer[BLE_rx_buffer.size] = '\0';
        // printf("**%s*%d*\n", BLE_rx_buffer.buffer, BLE_rx_buffer.size);
        if (BLE_rx_buffer.buffer[0] == '+') // 读取响应
        {
            if (BLE_rx_buffer.size == 8 && strcmp(BLE_rx_buffer.buffer, "+READY\r\n") == 0) {
                // printf("BLE_READY\r\n");
                osEventFlagsSet(globalEventHandle, BLE_FLAGS_READY);
            }
        } else if (BLE_rx_buffer.buffer[0] == BLE_CMD_BEGIN &&
                   BLE_rx_buffer.buffer[BLE_CMD_LENGTH - 1] == BLE_CMD_END) // 上位机发送指令
        {
            // for (uint16_t i = 0; i < BLE_rx_buffer.size; i++)printf("%c", BLE_rx_buffer.buffer[i]);
#if BLE_CMD_CHECKSUM == 1
            uint8_t sum = 0;
            for (int i = 0; i < BLE_CMD_LENGTH - 2; i++) sum += BLE_rx_buffer.buffer[i];
            // printf("BLE_CMD%#.2x\n,%d:%d", BLE_rx_buffer.buffer[1], sum, BLE_rx_buffer.buffer[BLE_CMD_LENGTH - 1]);
            if (sum == BLE_rx_buffer.buffer[BLE_CMD_LENGTH - 2]) {
#endif
                switch (BLE_rx_buffer.buffer[1]) {
                    case 0x00: // 重启指令
                        NVIC_SystemReset();
                    default:
                        CMD_Handler(BLE_rx_buffer.buffer[1], BLE_rx_buffer.buffer + 2);
                }
#if BLE_CMD_CHECKSUM == 1
            }
#endif
        } else {
            if (osMutexAcquire(BLESendMutexHandle, 0) != osOK) {
                // printf("BLE_W_BUSY\r\n");
                if (!strcmp(BLE_rx_buffer.buffer, "OK\r\n"))
                    osSemaphoreRelease(BLEWaitResultSemaphoreHandle);
                else {
                    if (!strcmp(BLE_rx_buffer.buffer, "ERROR\r\n"))
                        osSemaphoreRelease(BLEWaitResultSemaphoreHandle);
                    BLE_Rx_Error_Handler();
                }
            } else {
                osMutexRelease(BLESendMutexHandle);
                BLE_Rx_Error_Handler();
            }
        }

    }
    /* USER CODE END StartReceiveBLEData */
}