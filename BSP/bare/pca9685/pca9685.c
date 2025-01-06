/**
 * PCA9685舵机控制板
 * 使用 i2c2 总线
 * 双板一共控制18个电机，每个板使用 0 ~ 8口
*/

#include "pca9685.h"

uint8_t PCA9685_Send_Ready = 1;

double angles[2][16];
const PCA9685_Led_Info PCA9685_Led_Off_Range[2][16] = {
 // 舵机控制板 0
 {
  // 1号腿
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  // 2号腿
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  // 3号腿
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
 },
 // 舵机控制板 1
 {
  // 1号腿
  {109.4, 189.0},
  {109.4, 189.0},
  {115.4, 198.0},
  {102.4, 189.0},
  // 2号腿
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  // 3号腿
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
  {102.4, 189.0},
 }
};

/**
 * @brief 向PCA9685写入一个字节
 * @param slave 从机地址 7位
 * @param reg 寄存器地址
 * @param data 要写入的数据
 * @return 0: 正常  1: 错误
 */
uint8_t PCA9685_Write_Byte(const uint8_t slave, const uint8_t reg, uint8_t data)
{
 const HAL_StatusTypeDef ret =
  HAL_I2C_Mem_Write(PCA9685_HI2C, slave << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
 if (ret == HAL_OK)
  return 0;
 else
  return 1;
}


/**
 * @brief 向PCA9685读取一个字节
 * @param slave 从机地址 7位
 * @param reg 寄存器地址
 * @return 一个字节数据
 */
uint8_t PCA9685_Read_Byte(const uint8_t slave, const uint8_t reg)
{
 uint8_t res;
 HAL_I2C_Mem_Read(PCA9685_HI2C, slave << 1 | 1, reg, I2C_MEMADD_SIZE_8BIT, &res, 1, HAL_MAX_DELAY);
 return res;
}


/**
 * @brief 向PCA9685写入多个字节
 * @param salve 从机地址 7位
 * @param reg 寄存器地址
 * @param data 数据数组
 * @param len 数据长度
 * @return 0: 正常  1: 异常  2: 其他消息正在发送
 */
uint8_t PCA9685_aWrite_Len(const uint8_t salve, const uint8_t reg, uint8_t data[], const uint8_t len)
{
 if (!PCA9685_Send_Ready) return 2;
 PCA9685_Send_Ready = 0;
 const HAL_StatusTypeDef ret =
  // DMA通道被抢了
  HAL_I2C_Mem_Write_IT(PCA9685_HI2C, salve << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len);
 if (ret == HAL_OK)
  return 0;
 return 1;
}

/**
 * @brief 向PCA9685写入多个字节
 * @param salve 从机地址 7位
 * @param reg 寄存器地址
 * @param data 数据数组
 * @param len 数据长度
 * @return 0: 正常  1: 异常
 */
uint8_t PCA9685_Write_Len(const uint8_t salve, const uint8_t reg, uint8_t data[], const uint8_t len)
{
 // PCA9685_Send_Ready = 0;
 // const HAL_StatusTypeDef ret =
 //  HAL_I2C_Mem_Write(PCA9685_HI2C, salve << 1, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
 // PCA9685_Send_Ready = 1;
 HAL_StatusTypeDef ret =
  PCA9685_aWrite_Len(salve, reg, data, len);
 // 排队发送
 while (ret == 2) ret = PCA9685_aWrite_Len(salve, reg, data, len);
 if (ret == 1) return 1;
 // 等待发送完成
 while (PCA9685_Send_Ready != 1);
 return 0;
}


/**
 * @brief 初始化PCA9685
 * @attention 测试默认MODE1模式：0x11
 * @return 0 成功 2 设置频率失败 3 复位舵机失败
 */
uint8_t PCA9685_Init()
{
 uint8_t res;
 res = PCA9685_Write_Byte(PCA9685_ALL, PCA9685_MODE1, 0x21);
 if (res) return 1;
 res = PCA9685_Set_Freq(PCA9685_ALL, 50); // 设置PWM频率
 if (res) return 2;
 /*
#if PCA9685_ANGLE_SYMMETRICAL == 1
 res = PCA9685_Set_All_Angle(PCA9685_ALL, 0); // 复位舵机
#else
 res = PCA9685_Set_All_Angle(90); // 复位舵机
#endif
 if (res) return 3;
 */
 return 0;
}

/**
 * @brief 设置PWM频率
 * @param slave 从机地址
 * @param freq 频率
 * @return
 */
uint8_t PCA9685_Set_Freq(const uint8_t slave, double freq)
{
 uint8_t res = 0;

 freq *= 1.00; //说是经验值，可以增加精度
 const uint8_t pre_scale = 25000000.0 / 4096 / freq - 0.5; // 四舍五入

 const uint8_t old_mode = PCA9685_Read_Byte(slave, PCA9685_MODE1);

 //根据旧的模式值计算出新的模式值，将最高位清零（bit 7）并将第 5 位设为1（bit 4），表示将 PCA9685 设置为睡眠模式。
 const uint8_t new_mode = (old_mode & 0x7F) | 0x10;
 res |= PCA9685_Write_Byte(slave, PCA9685_MODE1, new_mode); //将新的模式值写入 PCA9685 的模式寄存器。
 res |= PCA9685_Write_Byte(slave, PCA9685_PRE_SCALE, pre_scale); //将计算得到的预分频器值写入 PCA9685 的预分频器寄存器。
 res |= PCA9685_Write_Byte(slave, PCA9685_MODE1, old_mode); //恢复旧的模式值。
 HAL_Delay(5); // 延时 5 毫秒，等待 PCA9685 完全启动。
 res |= PCA9685_Write_Byte(slave, PCA9685_MODE1, old_mode | 0xa1); //将模式值的最高位和第 1 位设为1，表示将 PCA9685 设置为正常工作模式。
 return res;
}

uint16_t PCA9685_Angle_To_Off(double angle, const uint8_t led)
{
 const uint8_t i = led >> 4, j = led & 0x0F;
#if PCA9685_ANGLE_SYMMETRICAL == 1
 angle += 90; // 角度对称模式下角度范围为 -90~90
#endif
 return PCA9685_Led_Off_Range[i][j].off + PCA9685_Led_Off_Range[i][j].range * angle / 90 + 0.5; // 四舍五入 并不完全参照理论，189.0为测量结果
 // return ((angle / 270.0) * (512 - 102)) + 102;
}

void _Set_Angle_Buffer(const uint8_t led, double angle, uint8_t sendBuffer[4])
{
#if PCA9685_ANGLE_SYMMETRICAL == 1
 if (angle < -90) angle = -90;
 else if (angle > 90) angle = 90;
#else
 if (angle < 0) angle = 0;
 else if (angle > 180) angle = 180;
#endif
 angles[led >> 4][led & 0x0F] = angle;
 const int off = PCA9685_Angle_To_Off(angle, led);
 sendBuffer[2] = off & 0xFF, sendBuffer[3] = off >> 8;
}

/**
 * @brief 设置舵机角度
 * @param slave 从机地址
 * @param led 0~3位舵机编号 0~15，4~7位控制板编号
 * @param angle 角度
 * @return
 */
uint8_t PCA9685_Set_Angle(const uint8_t slave, const uint8_t led, double angle)
{
 uint8_t sendBuffer[4] = {};
 _Set_Angle_Buffer(led, angle, sendBuffer);
 return PCA9685_Write_Len(slave, PCA9685_LEDX_ON_L(led & 0x0F), sendBuffer, 4);
}

/**
 * @brief 异步设置舵机角度
 * @param slave 从机地址
 * @param led 0~3位舵机编号 0~15，4~7位控制板编号
 * @param angle 角度
 * @return
 */
uint8_t PCA9685_aSet_Angle(const uint8_t slave, const uint8_t led, double angle)
{
 uint8_t sendBuffer[4] = {};
 _Set_Angle_Buffer(led, angle, sendBuffer);
 return PCA9685_Write_Len(slave, PCA9685_LEDX_ON_L(led & 0x0F), sendBuffer, 4);
}

void _Set_Angles_Buffer(const uint8_t slave_id, const double angles[16], uint8_t sendBuffer[4 * 16])
{
 for (uint8_t i = 0; i < 16; i++)
 {
  uint16_t off = PCA9685_Angle_To_Off(angles[i], slave_id << 4 | i);
  uint8_t L = off & 0xFF, H = off >> 8;
  sendBuffer[(i * 4) + 2] = L;
  sendBuffer[(i * 4) + 3] = H;
 }
}

uint8_t PCA9685_Set_Angles(const uint8_t slave_id, const double angles[16])
{
 uint8_t sendBuffer[4 * 16] = {};
 _Set_Angles_Buffer(slave_id, angles, sendBuffer);
 return PCA9685_Write_Len(slave_id == 0 ? PCA9685_0 : PCA9685_1, PCA9685_LEDX_ON_L(0), sendBuffer, 4 * 16);
}

uint8_t PCA9685_aSet_Angles(const uint8_t slave_id, const double angles[16])
{
 uint8_t sendBuffer[4 * 16] = {};
 _Set_Angles_Buffer(slave_id, angles, sendBuffer);
 return PCA9685_aWrite_Len(slave_id == 0 ? PCA9685_0 : PCA9685_1, PCA9685_LEDX_ON_L(0), sendBuffer, 4 * 16);
}

void _Set_Angles_Range_Buffer(const uint8_t slave_id, const double angles[], const uint8_t st, const uint8_t ed, uint8_t sendBuffer[])
{
 for (uint8_t i = st; i <= ed; i++)
 {
  uint16_t off = PCA9685_Angle_To_Off(angles[i - st], slave_id << 4 | i);
  uint8_t L = off & 0xFF, H = off >> 8;
  sendBuffer[(i - st) * 4 + 2] = L;
  sendBuffer[(i - st) * 4 + 3] = H;
 }
}

uint8_t PCA9685_Set_Angles_Range(const uint8_t slave_id,
                                 const double angles[], const uint8_t st, const uint8_t ed)
{
 uint8_t sendBuffer[4 * 16] = {};
 _Set_Angles_Range_Buffer(slave_id, angles, st, ed, sendBuffer);
 return PCA9685_Write_Len(slave_id == 0 ? PCA9685_0 : PCA9685_1, PCA9685_LEDX_ON_L(st), sendBuffer, 4 * (ed - st + 1));
}

uint8_t PCA9685_aSet_Angles_Range(const uint8_t slave_id,
                                  const double angles[], const uint8_t st, const uint8_t ed)
{
 uint8_t sendBuffer[4 * 16] = {};
 _Set_Angles_Range_Buffer(slave_id, angles, st, ed, sendBuffer);
 return PCA9685_Write_Len(slave_id == 0 ? PCA9685_0 : PCA9685_1, PCA9685_LEDX_ON_L(st), sendBuffer, 4 * (ed - st + 1));
}

void _Set_All_Angle_Buffer(const double angle, uint8_t* sendBuffer0, uint8_t* sendBuffer1)
{
 for (uint8_t i = 0; i < 2; i++)
  for (uint8_t j = 0; j < 16; j++)
   angles[i][j] = angle;
 // 这种方式似乎无法正常工作
 // uint8_t sendBuffer[4] = {0x00, 0x00, off & 0xFF, off >> 8};
 // return PCA9685_Write_Len(slave, PCA9685_ALL_LED_ON_H, sendBuffer, 4);
 // uint8_t sendBuffer[4 * 16] = {};
 for (uint8_t i = 0; i < 16; i++)
 {
  uint16_t off = PCA9685_Angle_To_Off(angle, i);
  uint8_t L = off & 0xFF, H = off >> 8;
  sendBuffer0[(i * 4) + 2] = L;
  sendBuffer0[(i * 4) + 3] = H;
  off = PCA9685_Angle_To_Off(angle, 0x10 | i);
  L = off & 0xFF, H = off >> 8;
  sendBuffer1[(i * 4) + 2] = L;
  sendBuffer1[(i * 4) + 3] = H;
 }
}

/**
 * @brief 设置所有舵机角度
 * @param angle 角度
 * @return
 */
uint8_t PCA9685_Set_All_Angle(const double angle)
{
 uint8_t sendBuffer0[4 * 16] = {}, sendBuffer1[4 * 16] = {};
 _Set_All_Angle_Buffer(angle, sendBuffer0, sendBuffer1);
 uint8_t ret;
 ret = PCA9685_Write_Len(PCA9685_0, PCA9685_LEDX_ON_L(0), sendBuffer0, 4 * 16);
 if (ret) return 1;
 ret = PCA9685_Write_Len(PCA9685_1, PCA9685_LEDX_ON_L(0), sendBuffer1, 4 * 16);
 if (ret) return 2;
 return 0;
}


/**
 * @brief 异步设置所有舵机角度，且不会等待
 * @param angle 角度
 * @return
 */
uint8_t PCA9685_aSet_All_Angle(const double angle)
{
 uint8_t sendBuffer0[4 * 16], sendBuffer1[4 * 16];
 _Set_All_Angle_Buffer(angle, sendBuffer0, sendBuffer1);
 // char s[100];
 // sprintf(s, "%d, %f", I2C1_aSend_Throttle_Counter, angle);
 // HAL_UART_Transmit(&huart1, s, strlen(s), HAL_MAX_DELAY);
 uint8_t ret;
 ret = PCA9685_aWrite_Len(PCA9685_0, PCA9685_LEDX_ON_L(0), sendBuffer0, 4 * 16);
 if (ret) return 1;
 ret = PCA9685_aWrite_Len(PCA9685_1, PCA9685_LEDX_ON_L(0), sendBuffer1, 4 * 16);
 if (ret) return 2;
 return 0;
}
