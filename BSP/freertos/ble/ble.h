/**
 * @brief MX-02 BLE Driver for FreeRTOS
 * @author syhanjin
 * @date 2024-09-23.
 */

#ifndef BLE_H
#define BLE_H
#include "cmsis_os.h"
#include "main.h"

#define __BLE_VERSION__ "0.0.2"

#define BLE_NAME        "DOG"
#define BLE_AMDATA      "FFAA0001"

#define BLE_CMD_LENGTH  9
#define BLE_CMD_BEGIN   0xAA
#define BLE_CMD_END     0xBB
/* 0.0.2 版本新增 */
/** 是否开启校验和校验
 * 协议头 **** 校验和 协议位
 * @attention 本校验和为直接求和对 0xFF 取模，不包括取反步骤
 */
#define BLE_CMD_CHECKSUM    1

#define BLE_RX_BUFFER_SIZE  48
#define BLE_W_CMD_TIMEOUT   10000

#define BLE_UART_HANDLE     (&huart2)

#define BLE_FLAGS_RECEIVE   (0x00000001U)
#define BLE_FLAGS_READY     (0x00000004U)

#define BLE_SIGNALS_RECEIVE (0x00000001U)

typedef struct
{
    uint8_t buffer[BLE_RX_BUFFER_SIZE];
    uint8_t size;
} BLE_Rx_t;

extern osThreadId_t receiveBLEDataHandle;
extern osMutexId_t BLESendMutexHandle;
extern osSemaphoreId_t BLEWaitResultSemaphoreHandle;

extern BLE_Rx_t BLE_rx_buffer;

uint8_t BLE_Init(void);
uint8_t BLE_Set_Name(char *name);
uint8_t BLE_Set_AMData(char *amData);

void BLE_Sleep(void);
void BLE_Wakeup(void);

void BLE_RxCpltCallback(void);
void CMD_Handler(uint8_t cmd, uint8_t data[]);

#endif // BLE_H
