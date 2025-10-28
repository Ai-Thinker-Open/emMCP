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
#ifndef _BSP_SHT30_H_
#define _BSP_SHT30_H_

#include "delay.h"
#include "main.h"

extern double Temperature, Humidity;

#define u8 unsigned char
#define delay_us(x) HAL_Delay_us(x)
#define delay_ms(x) HAL_Delay(x)
// 端口移植
#define RCC_SHT30 RCC_APB2Periph_GPIOB
#define PORT_SHT30 GPIOB

#define GPIO_SDA GPIO_PIN_6
#define GPIO_SCL GPIO_PIN_7

// SDA切换为输入模式（浮空输入）
#define SDA_IN()                                                               \
  {                                                                            \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                    \
    GPIO_InitStruct.Pin = GPIO_SDA;                                            \
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT; /* 浮空输入模式 */                 \
    GPIO_InitStruct.Pull = GPIO_NOPULL;                                        \
    HAL_GPIO_Init(PORT_SHT30, &GPIO_InitStruct);                               \
  }

// SDA切换为输出模式（推挽输出）
#define SDA_OUT()                                                              \
  {                                                                            \
    GPIO_InitTypeDef GPIO_InitStruct = {0};                                    \
    GPIO_InitStruct.Pin = GPIO_SDA;                                            \
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;   /* 推挽输出模式 */           \
    GPIO_InitStruct.Pull = GPIO_NOPULL;           /* 无上下拉 */               \
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; /* 高速模式（对应50MHz） */  \
    HAL_GPIO_Init(PORT_SHT30, &GPIO_InitStruct);                               \
  }
// 获取SDA引脚的电平变化
#define SDA_GET() HAL_GPIO_ReadPin(PORT_SHT30, GPIO_SDA)

// SDA输出电平控制（x为真时输出高电平，假时输出低电平）
#define SDA(x)                                                                 \
  HAL_GPIO_WritePin(PORT_SHT30, GPIO_SDA, (x ? GPIO_PIN_SET : GPIO_PIN_RESET))

// SCL输出电平控制（x为真时输出高电平，假时输出低电平）
#define SCL(x)                                                                 \
  HAL_GPIO_WritePin(PORT_SHT30, GPIO_SCL, (x ? GPIO_PIN_SET : GPIO_PIN_RESET))

void SHT30_GPIO_Init(void);
char SHT30_Read(uint16_t dat);

#endif