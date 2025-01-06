//
// Created by hanjin on 24-9-5.
//

#ifndef MX02S_H
#define MX02S_H

#define __MX02S_VERSION__ "0.0.1"

#define MX02S_CR 0x0D
#define MX02S_LF 0x0A

#define MX02S_NAME "HEXAPOD"
#define MX02S_AMDATA "FFAA0001"

#include "main.h"
#include "usart.h"
#include <string.h>

typedef enum
{
    MX02S_INITIALIZING = 0U,
    MX02S_READY = 1U,
} MX02S_StatusTypeDef;

typedef enum
{
    MX02S_S_NONE = 0U,
    MX02S_S_SENDING = 1U,
    MX02S_S_SUCCESS = 2U,
    MX02S_S_ERROR = 3U,
} MX02S_S_StatusTypeDef;

typedef enum
{
    MX02S_W_NONE = 0U,
    MX02S_W_WAITING = 1U,
    MX02S_W_SUCCESS = 2U,
    MX02S_W_ERROR = 3U,
} MX02S_W_StatusTypeDef;

extern MX02S_StatusTypeDef MX02S_Status;
extern MX02S_W_StatusTypeDef MX02S_W_Status;
extern MX02S_S_StatusTypeDef MX02S_S_Status;

extern uint8_t MX02S_Read_Buffer[128];

void MX02S_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t Size);
void MX02S_UART_TxCpltCallback(UART_HandleTypeDef* huart);

uint8_t MX02S_Init(void);

uint8_t MX02S_aSend_Data(char* data);
uint8_t MX02S_Send_Data(char* data);

uint8_t MX02S_Send_CMD(char* cmd);

uint8_t MX02S_Set_Name(char* name);
uint8_t MX02S_Set_AMData(char* amData);
uint8_t MX02S_Set_Adv_Status(const uint8_t status);

#endif //MX02S_H
