//
// Created by hanjin on 24-9-4.
//

#ifndef JY901S_H
#define JY901S_H

#define __JY901S_VERSION__ "0.0.1"

#define JY901S_SAVE        0x00  // 保存/重启/恢复出厂
#define JY901S_CALSW       0x01  // 校准模式
#define JY901S_RSW         0x02  // 输出内容
#define JY901S_RRATE       0x03  // 输出速率
#define JY901S_BAUD        0x04  // 串口波特率
#define JY901S_AXOFFSET    0x05  // 加速度X零偏
#define JY901S_AYOFFSET    0x06  // 加速度Y零偏
#define JY901S_AZOFFSET    0x07  // 加速度Z零偏
#define JY901S_GXOFFSET    0x08  // 角速度X零偏
#define JY901S_GYOFFSET    0x09  // 角速度Y零偏
#define JY901S_GZOFFSET    0x0A  // 角速度Z零偏
#define JY901S_HXOFFSET    0x0B  // 磁场X零偏
#define JY901S_HYOFFSET    0x0C  // 磁场Y零偏
#define JY901S_HZOFFSET    0x0D  // 磁场Z零偏
#define JY901S_D0MODE      0x0E  // D0引脚模式
#define JY901S_D1MODE      0x0F  // D1引脚模式
#define JY901S_D2MODE      0x10  // D2引脚模式
#define JY901S_D3MODE      0x11  // D3引脚模式
#define JY901S_IICADDR     0x1A  // IIC设备地址
#define JY901S_LEDOFF      0x1B  // 关闭LED灯
#define JY901S_MAGRANGX    0X1C  // 磁场X校准范围
#define JY901S_MAGRANGY    0X1D  // 磁场Y校准范围
#define JY901S_MAGRANGZ    0X1E  // 磁场Z校准范围
#define JY901S_BANDWIDTH   0x1F  // 带宽
#define JY901S_GYRORANGE   0x20  // 陀螺仪量程
#define JY901S_ACCRANGE    0x21  // 加速度量程
#define JY901S_SLEEP       0x22  // 休眠
#define JY901S_ORIENT      0x23  // 安装方向
#define JY901S_AXIS6       0x24  // 算法
#define JY901S_FILTK       0x25  // 动态滤波
#define JY901S_GPSBAUD     0x26  // GPS波特率
#define JY901S_READADDR    0x27  // 读取寄存器
#define JY901S_ACCFILT     0x2A  // 加速度滤波
#define JY901S_POWONSEND   0x2D  // 指令启动
#define JY901S_VERSION     0x2E  // 版本号
#define JY901S_YYMM        0x30  // 年月
#define JY901S_DDHH        0x31  // 时日
#define JY901S_MMSS        0x32  // 分秒
#define JY901S_MSL         0x33  // 毫秒
#define JY901S_AX          0x34  // 加速度X
#define JY901S_AY          0x35  // 加速度Y
#define JY901S_AZ          0x36  // 加速度Z
#define JY901S_GX          0x37  // 角速度X
#define JY901S_GY          0x38  // 角速度Y
#define JY901S_GZ          0x39  // 角速度Z
#define JY901S_HX          0x3A  // 磁场X
#define JY901S_HY          0x3B  // 磁场Y
#define JY901S_HZ          0x3C  // 磁场Z
#define JY901S_ROLL        0x3D  // 横滚角
#define JY901S_PITCH       0x3E  // 俯仰
#define JY901S_YAW         0x3F  // 航向角
#define JY901S_TEMP        0x40  // 温度
#define JY901S_D0STATUS    0x41  // D0引脚状态
#define JY901S_D1STATUS    0x42  // D1引脚状态
#define JY901S_D2STATUS    0x43  // D2引脚状态
#define JY901S_D3STATUS    0x44  // D3引脚状态
#define JY901S_PRESSURE_L  0x45  // 气压低16位
#define JY901S_PRESSURE_H  0x46  // 气压高16位
#define JY901S_HEIGHT_L    0x47  // 高度低16位
#define JY901S_HEIGHT_H    0x48  // 高度高16位
#define JY901S_LON_L       0x49  // 经度低16位
#define JY901S_LON_H       0x4A  // 经度高16位
#define JY901S_LAT_L       0x4B  // 纬度低16位
#define JY901S_LAT_H       0x4C  // 纬度高16位
#define JY901S_GPS_HEIGHT  0x4D  // GPS海拔
#define JY901S_GPS_YAW     0x4E  // GPS航向角
#define JY901S_GPS_V_L     0x4F  // GPS地速低16位
#define JY901S_GPS_V_H     0x50  // GPS地速高16位
#define JY901S_Q0          0x51  // 四元数0
#define JY901S_Q1          0x52  // 四元数1
#define JY901S_Q2          0x53  // 四元数2
#define JY901S_Q3          0x54  // 四元数3
#define JY901S_SVNUM       0x55  // 卫星数
#define JY901S_PDOP        0x56  // 位置精度
#define JY901S_HDOP        0x57  // 水平精度
#define JY901S_VDOP        0x58  // 垂直精度
#define JY901S_DELAYT      0x59  // 报警信号延迟
#define JY901S_XMIN        0x5A  // X轴角度报警最小值
#define JY901S_XMAX        0x5B  // X轴角度报警最大值
#define JY901S_BATVAL      0x5C  // 供电电压
#define JY901S_ALARMPIN    0x5D  // 报警引脚映射
#define JY901S_YMIN        0x5E  // Y轴角度报警最小值
#define JY901S_YMAX        0x5F  // Y轴角度报警最大值
#define JY901S_GYROCALITHR 0x61  // 陀螺仪静止阈值
#define JY901S_ALARMLEVEL  0x62  // 角度报警电平
#define JY901S_GYROCALTIME 0x63  // 陀螺仪自动校准时间
#define JY901S_TRIGTIME    0x68  // 报警连续触发时间
#define JY901S_KEY         0x69  // 解锁
#define JY901S_WERROR      0x6A  // 陀螺仪变化值
#define JY901S_TIMEZONE    0x6B  // GPS时区
#define JY901S_WZTIME      0x6E  // 角速度连续静止时间
#define JY901S_WZSTATIC    0x6F  // 角速度积分阈值
#define JY901S_MODDELAY    0x74  // 485数据应答延时
#define JY901S_XREFROLL    0x79  // 横滚角零位参考值
#define JY901S_YREFROLL    0x7A  // 俯仰角零位参考值
#define JY901S_NUMBERID1   0X7F  // 设备编号1-2
#define JY901S_NUMBERID2   0X80  // 设备编号3-4
#define JY901S_NUMBERID3   0X81  // 设备编号5-6
#define JY901S_NUMBERID4   0X82  // 设备编号7-8
#define JY901S_NUMBERID5   0X83  // 设备编号9-10
#define JY901S_NUMBERID6   0X84  // 设备编号11-12

#define g 9.8 // 重力加速度

#include "main.h"
#include "usart.h"

typedef enum
{
    JY901S_RESET = 0U,
    JY901S_RECEIVING = 1U
} JY901S_StatusTypeDef;

typedef struct
{
    uint8_t year, month, day, hour, minute, second;
    uint16_t millisecond;
    double ax, ay, az; // 加速度
    double gx, gy, gz; // 角度
    double hx, hy, hz; // 磁场
    double roll, pitch, yaw;
    double temperature;
    uint16_t version;
    int pressure, height;
    double q0, q1, q2, q3; // 四元数
} JY901S_DataTypeDef;

extern JY901S_DataTypeDef JY901S_Data;
extern JY901S_StatusTypeDef JY901S_Status;

uint8_t JY901S_Write(const uint8_t reg, const uint16_t data);
uint8_t JY901S_Send_CMD(const uint8_t reg, const uint16_t data);

uint8_t JY901S_Init(void);
uint8_t JY901S_GetData();

void JY901S_UART_RxCpltCallback(UART_HandleTypeDef* huart);

#endif //JY901S_H
