/**
 * @file ws2812.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-12-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "ws2812.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"
#include <stdint.h>

void ws2812_update(void) {
  uint16_t ws2812_data[] = {CODE1, CODE1, CODE1, CODE1,     CODE1, CODE1, CODE1,
                            CODE1, CODE0, CODE0, CODE0,     CODE0, CODE0, CODE0,
                            CODE0, CODE0, CODE0, CODE0,     CODE0, CODE0, CODE0,
                            CODE0, CODE0, CODE0, CODE_RESET};
  HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_1, (uint32_t*)ws2812_data,
                    sizeof(ws2812_data) / sizeof(uint16_t));
}