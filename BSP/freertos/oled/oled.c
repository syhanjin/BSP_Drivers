//
// Created by hanjin on 24-12-15.
//

#include "oled.h"

#include <stdio.h>

#ifdef SH1106
uint8_t OLED_page_index = 0;
#endif
uint8_t OLED_SRAM[8][128];

Player_t defaultPlayer;

void OLED_WriteByte(uint8_t data, GPIO_PinState dc)
{
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, dc);
    HAL_SPI_Transmit(OLED_SPI_HANDLE, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, OLED_DC_DATA);
}

void OLED_SendCMD(uint8_t cmd)
{
    OLED_WriteByte(cmd, OLED_DC_CMD);
}

void OLED_Reset(void)
{
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
    osDelay(10);
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);
}

#ifdef SSD1306
void OLED_StartRefresh(void)
{
    // OLED_SendCMD(0x22);
    // OLED_SendCMD(0x00);
    // OLED_SendCMD(0x07);
    // OLED_SendCMD(0xB0);

    // 开启 DMA 循环发送
    HAL_SPI_Transmit_DMA(OLED_SPI_HANDLE, (uint8_t *)OLED_SRAM, 8 * 128);
}

void OLED_StopRefresh(void)
{
    HAL_SPI_DMAStop(OLED_SPI_HANDLE);
}
#endif
#ifdef SH1106

void OLED_SetPage(uint8_t page)
{
    OLED_SendCMD(0xB0 + page);
}

void OLED_SetColumn(uint8_t column)
{
    column += 0x02;
    OLED_SendCMD(0x10 | (column >> 4));   // 设置列地址高位
    OLED_SendCMD(0x00 | (column & 0x0F)); // 设置列地址低位
}

void OLED_FreshPage(void)
{
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, OLED_DC_CMD);
    uint8_t data[] = {0xB0 + OLED_page_index, 0x10, 0x02};
    HAL_SPI_Transmit(OLED_SPI_HANDLE, data, 3, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, OLED_DC_DATA);
    HAL_SPI_Transmit_DMA(OLED_SPI_HANDLE, (uint8_t *)OLED_SRAM + OLED_page_index * 128, 128);
}

void OLED_Refresh(void)
{
    OLED_page_index = 0;
    OLED_FreshPage();
}
#endif

void OLED_DisplayOn(void)
{
#ifdef SSD1306
    OLED_SendCMD(0x8D); // 电荷泵使能
    OLED_SendCMD(0x14); // 开启电荷泵
    OLED_SendCMD(0xAF); // 点亮屏幕
#endif
#ifdef SH1106
    OLED_SendCMD(0x8B);
    OLED_SendCMD(0xAF);
#endif
}

void OLED_DisplayOff(void)
{
#ifdef SSD1306
    OLED_SendCMD(0x8D); // 电荷泵使能
    OLED_SendCMD(0x10); // 开启电荷泵
    OLED_SendCMD(0xAF); // 点亮屏幕
#endif
#ifdef SH1106
    OLED_SendCMD(0xAE);
    OLED_SendCMD(0x8A);
#endif
}

void OLED_Clear(void)
{
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 128; j++) OLED_SRAM[i][j] = 0;

#ifdef SH1106
    OLED_Refresh();
#endif
}

/**
 * @brief 初始化 OLED
 */
void OLED_Init(void)
{
    OLED_Reset();

#ifdef SSD1306
    OLED_SendCMD(0xAE); //--turn off oled panel
    OLED_SendCMD(0x00); //---set low column address
    OLED_SendCMD(0x10); //---set high column address
    OLED_SendCMD(0x40); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
    OLED_SendCMD(0x81); //--set contrast control register
    OLED_SendCMD(0xCF); // Set SEG Output Current Brightness
    OLED_SendCMD(0xA1); //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
    OLED_SendCMD(0xC8); // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
    OLED_SendCMD(0xA6); //--set normal display
    OLED_SendCMD(0xA8); //--set multiplex ratio(1 to 64)
    OLED_SendCMD(0x3F); //--1/64 duty
    OLED_SendCMD(0xD3); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
    OLED_SendCMD(0x00); //-not offset
    OLED_SendCMD(0xD5); //--set display clock divide ratio/oscillator frequency
    OLED_SendCMD(0x80); //--set divide ratio, Set Clock as 100 Frames/Sec
    OLED_SendCMD(0xD9); //--set pre-charge period
    OLED_SendCMD(0xF1); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
    OLED_SendCMD(0xDA); //--set com pins hardware configuration
    OLED_SendCMD(0x12);
    OLED_SendCMD(0xDB); //--set vcomh
    OLED_SendCMD(0x40); // Set VCOM Deselect Level
    OLED_SendCMD(0x20); //-Set Page Addressing Mode (0x00/0x01/0x02)
    OLED_SendCMD(0x00); //
    OLED_SendCMD(0x8D); //--set Charge Pump enable/disable
    OLED_SendCMD(0x14); //--set(0x10) disable
    OLED_SendCMD(0xA4); // Disable Entire Display On (0xa4/0xa5)
    OLED_SendCMD(0xA6); // Disable Inverse Display On (0xa6/a7)
    OLED_SendCMD(0xAF);
#endif
#ifdef SH1106
    OLED_SendCMD(0xAE); /*display off*/
    OLED_SendCMD(0x02); /*set lower column address*/
    OLED_SendCMD(0x10); /*set higher column address*/
    OLED_SendCMD(0x40); /*set display start line*/
    OLED_SendCMD(0xB0); /*set page address*/
    OLED_SendCMD(0x81); /*contract control*/
    OLED_SendCMD(0xcf); /*128*/
    OLED_SendCMD(0xA1); /*set segment remap*/
    OLED_SendCMD(0xA6); /*normal / reverse*/
    OLED_SendCMD(0xA8); /*multiplex ratio*/
    OLED_SendCMD(0x3F); /*duty = 1/64*/
    OLED_SendCMD(0xad); /*set charge pump enable*/
    OLED_SendCMD(0x8b); /* 0x8B 内供 VCC */
    OLED_SendCMD(0x33); /*0X30---0X33 set VPP 9V */
    OLED_SendCMD(0xC8); /*Com scan direction*/
    OLED_SendCMD(0xD3); /*set display offset*/
    OLED_SendCMD(0x00); /* 0x20 */
    OLED_SendCMD(0xD5); /*set osc division*/
    OLED_SendCMD(0x80);
    OLED_SendCMD(0xD9); /*set pre-charge period*/
    OLED_SendCMD(0x1f); /*0x22*/
    OLED_SendCMD(0xDA); /*set COM pins*/
    OLED_SendCMD(0x12);
    OLED_SendCMD(0xdb); /*set vcomh*/
    OLED_SendCMD(0x40);
    OLED_SendCMD(0xAF); /*display ON*/
#endif

    osDelay(50);
#ifdef SSD1306
    OLED_StartRefresh();
#endif

#ifdef SH1106
    OLED_Refresh();
#endif
    osEventFlagsSet(globalEventHandle, OLED_FLAGS_READY);
}

/* 图形库 */
/**
 * @brief 绘制直线
 * 从 (x1, y1) 到 (x2, y2)
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 */
void OLED_DrawLine(int8_t x1, int8_t y1, int8_t x2, int8_t y2)
{
    if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
        x1 > 127 || x2 > 127 || y1 > 63 || y2 > 63)
        return;
    int8_t x, y;
    if (x1 > x2) {
        x = x2, x2 = x1, x1 = x;
        y = y2, y2 = y1, y1 = y;
    }
    if (y1 <= y2) {
        if (y2 - y1 <= x2 - x1) {
            float k = (float)(y2 - y1) / (float)(x2 - x1), e = -0.5;
            for (x = x1, y = y1; x < x2; x++) {
                __OLED_SET_POINT(x, y);
                e += k;
                if (e > 0) {
                    y++;
                    e -= 1;
                }
            }
            __OLED_SET_POINT(x2, y2);
        } else {
            float k = (float)(x2 - x1) / (float)(y2 - y1), e = -0.5;
            for (x = x1, y = y1; y < y2; y++) {
                __OLED_SET_POINT(x, y);
                e += k;
                if (e > 0) {
                    x++;
                    e -= 1;
                }
            }
            __OLED_SET_POINT(x2, y2);
        }
    } else {
        if (y1 - y2 <= x2 - x1) {
            float k = (float)(y2 - y1) / (float)(x2 - x1), e = 0.5;
            for (x = x1, y = y1; x < x2; x++) {
                __OLED_SET_POINT(x, y);
                e += k;
                if (e < 0) {
                    y--;
                    e += 1;
                }
            }
            __OLED_SET_POINT(x2, y2);
        } else {
            float k = (float)(x2 - x1) / (float)(y2 - y1), e = 0.5;
            for (x = x1, y = y1; y < y2; y++) {
                __OLED_SET_POINT(x, y);
                e += k;
                if (e < 0) {
                    x--;
                    e += 1;
                }
            }
            __OLED_SET_POINT(x2, y2);
        }
    }
}

/**
 * @brief 绘制与坐标轴平行的矩形
 * @param x 起点 x
 * @param y 起点 y
 * @param w 宽度（x 方向）
 * @param h 高度（y 方向）
 */
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t fill)
{
    if (x > 127 || y > 63 || w > 127 || h > 63 || x + w > 127 || y + h > 63) return;
    if (fill) {
        for (int i = y; i <= y + h; i++)
            for (int j = x; j <= x + w; j++)
                __OLED_SET_POINT(j, i);
    } else {
        for (int i = y; i <= y + h; i++) {
            __OLED_SET_POINT(x, i);
            __OLED_SET_POINT(x + w, i);
        }
        for (int j = x + 1; j < x + w; j++) {
            __OLED_SET_POINT(j, y);
            __OLED_SET_POINT(j, y + h);
        }
    }
}

/**
 * @brief 画圆
 * @param xc
 * @param yc
 * @param radius
 */
void OLED_DrawCircle(uint8_t xc, uint8_t yc, uint8_t radius)
{
    uint8_t x = radius;
    uint8_t y = 0;
    int16_t p = 3 - 2 * radius; // 初始判断参数

    // 绘制第一个象限的点，然后通过对称性绘制其余部分
    while (x >= y) {
        // 8个对称点
        __OLED_SET_POINT(xc + x, yc + y);
        __OLED_SET_POINT(xc - x, yc + y);
        __OLED_SET_POINT(xc + x, yc - y);
        __OLED_SET_POINT(xc - x, yc - y);
        __OLED_SET_POINT(xc + y, yc + x);
        __OLED_SET_POINT(xc - y, yc + x);
        __OLED_SET_POINT(xc + y, yc - x);
        __OLED_SET_POINT(xc - y, yc - x);

        y++;

        // 根据当前的 p 判断如何更新 x 和 y
        if (p <= 0) {
            p += 4 * y + 6;
        } else {
            x--;
            p += 4 * (y - x) + 10;
        }
    }
}

/**
 * @brief 显示字模点阵
 * @param x 左上角坐标
 * @param y 左上角坐标
 * @param w 宽
 * @param h 高
 * @param matrix 点阵 每一位 从低位到高位 到 w 后换入下一行
 */
void OLED_ShowMatrix(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *matrix)
{
    // TODO: 提升效率
    for (uint8_t j = 0; j < (h + 7) / 8; j++)
        for (uint8_t i = 0; i < w; i++) {
            uint8_t temp = matrix[i + j * w];
            for (uint8_t k = 0; k < 8; k++) {
                const uint8_t px = x + i, py = y + j * 8 + k;
                if (px > 127 || py > 63) continue;
                if (temp & 0x01)
                    __OLED_SET_POINT(px, py);
                else
                    __OLED_RESET_POINT(px, py);
                temp = temp >> 1;
            }
        }
}

/**
 * @brief 展示图片
 * @param x
 * @param y
 * @param image
 * @note Main TopToBottom, Line Forward, Bandwidth 8px, 先高位后低位
 */
void OLED_ShowPicture(uint8_t x, uint8_t y, const tImage *image)
{
    if (image == NULL) return;
    for (uint8_t j = 0; j < (image->height + 7) / 8; j++)
        for (uint8_t i = 0; i < image->width; i++) {
            uint8_t temp = image->data[i + j * image->width];
            for (uint8_t k = 0; k < 8; k++) {
                const uint8_t px = x + i, py = y + j * 8 + k;
                if (px > 127 || py > 63) continue;
                if (temp & 0x80)
                    __OLED_SET_POINT(px, py);
                else
                    __OLED_RESET_POINT(px, py);
                temp = temp << 1;
            }
        }
}

/*
void OLED_DrawObliqueRect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, uint8_t fill)
{
    uint8_t x4 = x1 + x3 - x2, y4 = y1 + y3 - y2;
    if (x1 > 127 || x2 > 127 || x3 > 127 || x4 > 127 ||
        y1 > 63 || y2 > 63 || y3 > 63 || y4 > 63) // 注：不会因为溢出导致错误判断
        return;
    if (fill) {} else {
        OLED_DrawLine(x1, y1, x2, y2);
        OLED_DrawLine(x2, y2, x3, y3);
        OLED_DrawLine(x3, y3, x4, y4);
        OLED_DrawLine(x4, y4, x1, y1);

    }
}
*/

/**
 * @brief 清除与坐标轴平行的矩形区域
 * @param x 起点 x
 * @param y 起点 y
 * @param w 宽度（x 方向）
 * @param h 高度（y 方向）
 */
void OLED_CleanRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    if (x > 127 || y > 63 || w > 127 || h > 63 || x + w > 127 || y + h > 63) return;
    for (int i = y; i <= y + h; i++)
        for (int j = x; j <= x + w; j++)
            __OLED_RESET_POINT(j, i);
}

void OLED_PlayVideo(uint8_t x, uint8_t y,
                    int16_t count, uint16_t interval, uint16_t time,
                    const Video_t *video)
{
    if (video == NULL) return;
    /* 打断当前播放 */
    osEventFlagsSet(globalEventHandle, OLED_FLAGS_PLAYER_INTERRUPT);

    /* 锁定帧 */
    osMutexAcquire(oledPlayMutexHandle, osWaitForever);

    /* 清除打断事件，避免多次打断
     * 注：不在播放间隔时不会等待打断事件此时切换视频打断不会被处理，故需要手动清理
     */
    osEventFlagsClear(globalEventHandle, OLED_FLAGS_PLAYER_INTERRUPT);

    /* 设置当前播放的视频 */
    defaultPlayer.video      = video;
    defaultPlayer.count      = count;
    defaultPlayer.x          = x;
    defaultPlayer.y          = y;
    defaultPlayer.frameIndex = 0;
    defaultPlayer.interval   = interval;
    defaultPlayer.time       = time;

    /* 释放帧 */
    osMutexRelease(oledPlayMutexHandle);

    /* 将播放器设置为播放状态 */
    osEventFlagsSet(globalEventHandle, OLED_FLAGS_PLAYER_BUSY);
}

/**
 * OLED 播放器，播放视频
 * @param argument
 */
void StartOLEDPlayer(void *argument)
{
    /* USER CODE BEGIN StartOLEDPlayer */
    // printf("OLED Player Inited");
    osDelay(100);
    /* Infinite loop */
    for (;;) {
        /* 等待播放器处于播放状态 */
        osEventFlagsWait(globalEventHandle, OLED_FLAGS_PLAYER_BUSY, osFlagsWaitAll | osFlagsNoClear, osWaitForever);

        /* 计数归 0 则停止播放 */
        if (defaultPlayer.count == 0) {
            osEventFlagsClear(globalEventHandle, OLED_FLAGS_PLAYER_BUSY);
            continue;
        }

        /* 锁定当前帧 */
        osMutexAcquire(oledPlayMutexHandle, osWaitForever);

        /* 播放帧 */
        OLED_ShowPicture(defaultPlayer.x, defaultPlayer.y, defaultPlayer.video->frames[defaultPlayer.frameIndex]);

#ifdef SH1106
        /* 刷新屏幕 */
        OLED_Refresh();
#endif

        /* 等待一帧时间 */
        // v0.0.4 取消 fps 改为设置播放时间
        osDelay(defaultPlayer.time / defaultPlayer.video->frameCount);

        /* 进入下一帧 */
        defaultPlayer.frameIndex++;
        if (defaultPlayer.frameIndex == defaultPlayer.video->frameCount) {
            if (defaultPlayer.count > 0) defaultPlayer.count--;
            defaultPlayer.frameIndex = 0;
            // 如果设定了播放间隔，且未播放结束，则等待一个间隔
            if (defaultPlayer.interval > 0 && defaultPlayer.count != 0) {
                /* 等待一个打断信号并设置超时时间为等待时间
                 * 接收到打断信号后停止播放
                 */
                if(osEventFlagsWait(globalEventHandle,OLED_FLAGS_PLAYER_INTERRUPT, osFlagsWaitAll, defaultPlayer.interval) == osOK){
                    osEventFlagsClear(globalEventHandle, OLED_FLAGS_PLAYER_BUSY);
                }
            }

        }
        /* 释放当前帧 */
        osMutexRelease(oledPlayMutexHandle);
    }
    /* USER CODE END StartOLEDPlayer */
}
