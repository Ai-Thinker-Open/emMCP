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

#define LED_NUM 8
void ws2812_update(void);
#endif