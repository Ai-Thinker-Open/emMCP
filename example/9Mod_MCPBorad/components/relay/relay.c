/**
 * @file relay.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-30
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "relay.h"
#include "gpio.h"
#include "log.h"
#include "stm32f1xx_hal_gpio.h"
#include <sys/_types.h>

static unsigned char axk_relay_state = 0;

void axk_relay_init(void) {
  // 判断 PB5 是否为G
  if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) != GPIO_PIN_RESET) {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
    return;
  }
}

void axk_relay_set(unsigned char state) {
  axk_relay_state = state;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, state);
}

void axk_relay_toggle(void) {
  axk_relay_state = !axk_relay_state;
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, axk_relay_state);
}
unsigned char axk_relay_get(void) { return axk_relay_state; }