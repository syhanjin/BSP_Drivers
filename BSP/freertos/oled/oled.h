/**
 * @brief OLED Driver for FreeRTOS
 * v0.0.1 版本驱动的移植版本，适应 FreeRTOS
 * v0.0.3 版本兼容了 SSD1306 和 SH1106
 * @attention SSD1306 需要将 DMA 设置为循环模式；SH1106 需要将 DMA 设置为普通模式, 且需要手动调用刷新
 * @author syhanjin
 * @date 2024-12-15.
 */

#ifndef OLED_H
#define OLED_H
#include "main.h"
#include "spi.h"
#include "gpio.h"

#define __OLED_VERSION__ "0.0.5"

/** 设置驱动芯片
 * 如果使用 SSD1306 将 DMA 开启为循环模式
 * 如果使用 SH1106  将 DMA 开启为正常模式
 **/
// #define SSD1306
#define SH1106

#define OLED_SPI_HANDLE             (&hspi2)

#define OLED_DC_CMD                 (GPIO_PIN_RESET)
#define OLED_DC_DATA                (GPIO_PIN_SET)

#define OLED_FLAGS_READY            (0x00000010U)
#define OLED_FLAGS_PLAYER_BUSY      (0x00000020U)
#define OLED_FLAGS_PLAYER_INTERRUPT (0x00000040U)

#define MAX_VIDEO_FRAMES            (16U)

typedef struct
{
    const uint8_t *data;
    uint16_t width;
    uint16_t height;
    uint8_t dataSize;
} tImage;

typedef struct
{
    const tImage *frames[MAX_VIDEO_FRAMES];
    uint16_t frameCount;
    uint16_t width;
    uint16_t height;
} Video_t;

typedef struct
{
    const Video_t *video; ///< 视频资源
    uint16_t frameIndex;
    uint8_t x, y;      ///< 坐标
    uint16_t time;     ///< 一次播放用时
    uint16_t interval; ///< 两次播放间隔
    int16_t count;     ///< 播放次数，-1 表示无限循环
} Player_t;

extern uint8_t OLED_SRAM[8][128];

extern Player_t defaultPlayer;

extern osMutexId_t oledPlayMutexHandle;
extern osThreadId_t OLEDPlayerHandle;

/**
 * @brief 点亮一个点
 * 左上角为原点，水平向右为 x 轴正向
 * @param __X__
 * @param __Y__
 */
#define __OLED_SET_POINT(__X__, __Y__) (OLED_SRAM[((__Y__) / 8)][__X__] |= 1U << ((__Y__) % 8))
/**
 * 清除一个点
 * @param __X__
 * @param __Y__
 */
#define __OLED_RESET_POINT(__X__, __Y__) (OLED_SRAM[((__Y__) / 8)][__X__] &= ~(1U << ((__Y__) % 8)))

void OLED_Init(void);
#ifdef SSD1306
void OLED_StartRefresh(void);
void OLED_StopRefresh(void);
#endif

#ifdef SH1106
extern uint8_t OLED_page_index;
void OLED_FreshPage(void);
void OLED_SetPage(uint8_t page);
void OLED_SetColumn(uint8_t column);
void OLED_Refresh(void);
#endif

/** 0.0.5 版本新增
 * 可控制 OLED 屏幕的开启与关闭
 * @attention OLED_DisplayOff 是关闭屏幕并让 OLED 进入睡眠模式
 */
void OLED_DisplayOn(void);
void OLED_DisplayOff(void);

void OLED_DrawLine(int8_t x1, int8_t y1, int8_t x2, int8_t y2);
void OLED_DrawRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t fill);
void OLED_DrawCircle(uint8_t xc, uint8_t yc, uint8_t radius);
void OLED_ShowMatrix(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t *matrix);
void OLED_ShowPicture(uint8_t x, uint8_t y, const tImage *image);
void OLED_PlayVideo(uint8_t x, uint8_t y, int16_t count, uint16_t interval, uint16_t time, const Video_t *video);

void OLED_CleanRect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);

#endif //OLED_H
