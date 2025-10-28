/*
 * 立创开发板软硬件资料与相关扩展板软硬件资料官网全部开源
 * 开发板官网：www.lckfb.com
 * 技术支持常驻论坛，任何技术问题欢迎随时交流学习
 * 立创论坛：https://oshwhub.com/forum
 * 关注bilibili账号：【立创开发板】，掌握我们的最新动态！
 * 不靠卖板赚钱，以培养中国工程师为己任
 * Change Logs:
 * Date           Author       Notes
 * 2024-03-29     LCKFB-LP    first version
 */

#include "bsp_sht30.h"
#include "stdio.h"
#include "stm32_hal_legacy.h"
#include "stm32f1xx_hal_gpio.h"

double Temperature = 0.0, Humidity = 0.0;

/******************************************************************
 * 函 数 名 称：SHT30_GPIO_Init
 * 函 数 说 明：SHT30的引脚初始化
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void SHT30_GPIO_Init(void) {
  // 使能GPIO端口时钟（需根据实际PORT_SHT30定义修改，例如GPIOB对应RCC_PERIPH_GPIOB）
  __HAL_RCC_GPIOB_CLK_ENABLE(); // 注意：需替换为实际端口的时钟使能宏，参考下方说明

  GPIO_InitTypeDef GPIO_InitStructure = {0};

  GPIO_InitStructure.Pin = GPIO_SDA | GPIO_SCL;    // SDA和SCL引脚
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;   // 推挽输出模式
  GPIO_InitStructure.Pull = GPIO_NOPULL;           // 无上下拉
  GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 高速模式（对应50MHz）

  HAL_GPIO_Init(PORT_SHT30, &GPIO_InitStructure); // HAL库GPIO初始化函数
}

/******************************************************************
 * 函 数 名 称：IIC_Start
 * 函 数 说 明：IIC起始时序
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void IIC_Start(void) {
  SDA_OUT();
  SCL(1);
  SDA(0);

  SDA(1);
  delay_us(5);
  SDA(0);
  delay_us(5);

  SCL(0);
}
/******************************************************************
 * 函 数 名 称：IIC_Stop
 * 函 数 说 明：IIC停止信号
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void IIC_Stop(void) {
  SDA_OUT();
  SCL(0);
  SDA(0);

  SCL(1);
  delay_us(5);
  SDA(1);
  delay_us(5);
}

/******************************************************************
 * 函 数 名 称：IIC_Send_Ack
 * 函 数 说 明：主机发送应答或者非应答信号
 * 函 数 形 参：0发送应答  1发送非应答
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void IIC_Send_Ack(unsigned char ack) {
  SDA_OUT();
  SCL(0);
  SDA(0);
  delay_us(5);
  if (!ack)
    SDA(0);
  else
    SDA(1);
  SCL(1);
  delay_us(5);
  SCL(0);
  SDA(1);
}

/******************************************************************
 * 函 数 名 称：I2C_WaitAck
 * 函 数 说 明：等待从机应答
 * 函 数 形 参：无
 * 函 数 返 回：0有应答  1超时无应答
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
unsigned char I2C_WaitAck(void) {

  char ack = 0;
  unsigned char ack_flag = 10;
  SCL(0);
  SDA(1);
  SDA_IN();

  SCL(1);
  while ((SDA_GET() == 1) && (ack_flag)) {
    ack_flag--;
    delay_us(5);
  }

  if (ack_flag <= 0) {
    IIC_Stop();
    return 1;
  } else {
    SCL(0);
    SDA_OUT();
  }
  return ack;
}

/******************************************************************
 * 函 数 名 称：Send_Byte
 * 函 数 说 明：写入一个字节
 * 函 数 形 参：dat要写人的数据
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
void Send_Byte(u8 dat) {
  int i = 0;
  SDA_OUT();
  SCL(0); // 拉低时钟开始数据传输

  for (i = 0; i < 8; i++) {
    SDA((dat & 0x80) >> 7);
    delay_us(1);
    SCL(1);
    delay_us(5);
    SCL(0);
    delay_us(5);
    dat <<= 1;
  }
}

/******************************************************************
 * 函 数 名 称：Read_Byte
 * 函 数 说 明：IIC读时序
 * 函 数 形 参：无
 * 函 数 返 回：读到的数据
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
unsigned char Read_Byte(void) {
  unsigned char i, receive = 0;
  SDA_IN(); // SDA设置为输入
  for (i = 0; i < 8; i++) {
    SCL(0);
    delay_us(5);
    SCL(1);
    delay_us(5);
    receive <<= 1;
    if (SDA_GET()) {
      receive |= 1;
    }
    delay_us(5);
  }
  SCL(0);
  return receive;
}

/******************************************************************
 * 函 数 名 称：SHT31_Write_mode
 * 函 数 说 明：在周期模式下设置测量周期与可重复性命令
 * 函 数 形 参：dat设置命令
                常用的有：每一秒采集0.5次  0x2024
                          每一秒采集1次    0x2126
                          每一秒采集2次    0x2220
                          每一秒采集4次    0x2334
                          每一秒采集10次   0x2721
 * 函 数 返 回：
 * 作       者：LC
 * 备       注：
******************************************************************/
char SHT31_Write_mode(uint16_t dat) {
  IIC_Start();

  // << 1 是将最后一位置0，设置为写命令
  Send_Byte((0X44 << 1) | 0);
  // 返回0为产生了应答，返回1说明通信失败
  if (I2C_WaitAck() == 1)
    return 1;
  // 发送命令的高8位
  Send_Byte((dat >> 8));
  // 返回0为产生了应答，返回1说明通信失败
  if (I2C_WaitAck() == 1)
    return 2;
  // 发送命令的低8位
  Send_Byte(dat & 0xff);
  // 返回0为产生了应答，返回1说明通信失败
  if (I2C_WaitAck() == 1)
    return 3;
  //        IIC_Stop();
  return 0;
}

/******************************************************************
 * 函 数 名 称：crc8
 * 函 数 说 明：CRC校验
 * 函 数 形 参：data要校验的数据地址     len要校验的长度
 * 函 数 返 回：校验后的值
 * 作       者：LC
 * 备       注：无
 ******************************************************************/
unsigned char crc8(const unsigned char *data, int len) {
  const unsigned char POLYNOMIAL = 0x31;
  unsigned char crc = 0xFF;
  int j, i;

  for (j = 0; j < len; j++) {
    crc ^= *data++;
    for (i = 0; i < 8; i++) {
      crc = (crc & 0x80) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

/******************************************************************
 * 函 数 名 称：SHT30_Read
 * 函 数 说 明：读取温湿度值
 * 函 数 形 参：dat读取的命令
                周期模式命令为：0xe000
                单次模式命令为：0x2c06 or 0x2400
 * 函 数 返 回：0读取成功  其他失败
 * 作       者：LC
 * 备       注：当前为周期模式读取，如使用单次模式，则将
 *              【设置周期模式命令】下的命令注释即可。
******************************************************************/
char SHT30_Read(uint16_t dat) {
  uint16_t i = 0;
  unsigned char buff[6] = {0};
  uint16_t data_16 = 0;

  // 设置周期模式命令
  // SHT31_Write_mode(0x2130); // 每1秒一次高重复测量(需要在周期模式下才有用)

  IIC_Start();
  Send_Byte((0x44 << 1) | 0);
  if (I2C_WaitAck() == 1)
    return 1;
  Send_Byte((dat >> 8));
  if (I2C_WaitAck() == 1)
    return 2;
  Send_Byte(dat & 0xff);
  if (I2C_WaitAck() == 1)
    return 3;

  // 如不使用超时判断，很容易数据错乱
  do {
    // 超时判断
    i++;
    if (i > 20)
      return 4;
    delay_ms(10);

    IIC_Start();
    Send_Byte((0X44 << 1) | 1); // 读

  } while (I2C_WaitAck() == 1); // 读取到温湿度数据则结束读命令

  // 温度高8位
  buff[0] = Read_Byte();
  IIC_Send_Ack(0);
  // 温度低8位
  buff[1] = Read_Byte();
  IIC_Send_Ack(0);
  // 温度CRC校验值
  buff[2] = Read_Byte();
  IIC_Send_Ack(0);
  // 湿度高8位
  buff[3] = Read_Byte();
  IIC_Send_Ack(0);
  // 湿度低8位
  buff[4] = Read_Byte();
  IIC_Send_Ack(0);
  // 湿度CRC校验值
  buff[5] = Read_Byte();
  IIC_Send_Ack(1);

  IIC_Stop();

  // CRC校验（将要校验的数值带入，查看计算后的校验值是否和读取到的校验值一致）
  if ((crc8(buff, 2) == buff[2]) && (crc8(buff + 3, 2) == buff[5])) {
    // 计算温度值
    data_16 = (buff[0] << 8) | buff[1];
    Temperature = (data_16 / 65535.0) * 175.0 - 45;
    // 计算湿度值
    data_16 = 0;
    data_16 = (buff[3] << 8) | buff[4];
    Humidity = (data_16 / 65535.0) * 100.0;

    return 0;
  } else {

    return 5;
  }

  return 0;
}