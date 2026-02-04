/**
 * @file ws2812.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __WS2812_H
#define __WS2812_H

#include "gpio.h"
#include "main.h"
#define CODE0 30
#define CODE1 60
#define CODE_RESET 0

#define WS2812_LED_NUM 60 // 默认支持60个LED
#define WS2812_BUFFER_SIZE                                                     \
  (WS2812_LED_NUM * 24 + 50) // 每个LED需要24个时序位 + 重置时间

// 颜色结构体定义
typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} ws2812_color_t;

// 全局颜色缓冲区
extern ws2812_color_t ws2812_leds[WS2812_LED_NUM];

// 函数声明
void ws2812_init(void);
void ws2812_update(void);
void ws2812_set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b);
void ws2812_fill(uint8_t r, uint8_t g, uint8_t b);
void ws2812_update_with_num(uint16_t led_num);
#endif