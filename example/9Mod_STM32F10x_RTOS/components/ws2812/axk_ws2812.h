/**
 * @file axk_ws2812.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_WS2812_H__
#define __AXK_WS2812_H_

#if __has_include("stm32f10x_pwm_dma.h")

#include "stm32f10x_pwm_dma.h"
// 定义I2C操作函数
#define AXK_WS2812_ACLL(_func, ...) bsp_pwm_dma_##_func(__VA_ARGS__)
#pragma message "stm32f10x_pwm_dma.h included for WS2812 I2C operations"
#else
#error "Please include the appropriate PWM_DMA header for WS2812."
#endif

#endif
