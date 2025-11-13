/**
 * @file digital_ws2812.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __DIGITAL_WS2812_H
#define __DIGITAL_WS2812_H

#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "ws2812.h"

#define WS2812_LED_NUM 3 // 支持最多24个WS2812 LED

void ws281x_digital_init(ws2812_strip_t *ws2812_strip);
void ws281x_digital_show_leds(void);
void ws281x_digital_set_pixel_color(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue) ;

#endif