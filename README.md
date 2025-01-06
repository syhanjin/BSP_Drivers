# BSP_Drivers

![GitHub License](https://img.shields.io/github/license/syhanjin/BSP_Drivers)

---

因为学 STM32 之后有个坏习惯，总喜欢按自己的喜好手搓一套驱动再用，故收集整理到一起作为一个仓库

欢迎提交 PR 和 Issue

## 驱动列表

| 驱动名称  | 平台     | 描述                                                         | 版本  | 教程下载                                                     | 状态     |
| --------- | -------- | ------------------------------------------------------------ | ----- | ------------------------------------------------------------ | -------- |
| motor.h   | 裸机     | 基于 `TB6612` 控制芯片的直流电机驱动                         | 0.0.2 | [教程](https://syhanjin.moe/20241201/ce368667e188/) [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/motor/) |          |
| pid.h     | 裸机     | 适应直流电机的 `PID` 控制驱动                                | 0.3.2 | [教程](https://syhanjin.moe/20241201/ce368667e188/) [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/pid/) |          |
| omni4.h   | 裸机     | 四轮全向轮底盘驱动（基于 `motor.h/pid.h`）                   | 0.0.1 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/omni4/) |          |
| pca9685.h | 裸机     | 通过 `PCA9685` 舵机驱动板控制舵机的驱动                      | 0.0.1 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/pca9685/) |          |
| mg92b.h   | 裸机     | 直接驱动 `MG92B` 舵机的舵机驱动（理论上兼容所有 PWM 舵机）   | 0.1.0 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/mg92b/) | 不再维护 |
| mpu6050.h | 裸机     | 因为 `MPU6050` 质量不好导致调了一个下午的 `MPU6050` 陀螺仪驱动 | 0.0.1 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/mpu6050/) | 不再维护 |
| mx02s.h   | 裸机     | `MX-02` 系列蓝牙模块驱动（深圳妙享科技）                     | 0.0.1 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/mx02s/) |          |
| jy901s.h  | 裸机     | `JY901S` 陀螺仪驱动（深圳维特智能）                          | 0.0.1 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/bare/jy901s/) |          |
| ble.h     | FreeRTOS | 基于 `FreeRTOS` 的 `MX-02` 系列蓝牙模块驱动移植版本          | 0.0.2 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/freertos/ble/) | 正在开发 |
| oled.h    | FreeRTOS | `OLED` 驱动，支持 `SSD1306` 和 `SH1106` 双驱动芯片，`SPI` 协议通信 | 0.0.5 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/freertos/oled/) | 正在开发 |
| servo.h   | FreeRTOS | 基于 `FreeRTOS` 的舵机驱动 `PWM` 舵机                        | 0.0.2 | [下载](https://github.com/syhanjin/BSP_Drivers/tree/master/src/BSP/freertos/servo/) | 正在开发 |

