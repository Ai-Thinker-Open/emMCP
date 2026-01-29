/**
 * @file ch224_i2c.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "ch224_i2c.h"
#include <stdint.h>
/**
 * @brief 初始化CH224 I2C
 *
 * @return uint8_t 0:成功 1:失败
 */
uint8_t ch224_i2c_init(void) {
  // 初始化I2C GPIO
  __CH224_FUNC_ALL(gpio_init);
  // 初始化I2C外设
  __CH224_FUNC_ALL(init);
  // 发送起始信号
  __CH224_FUNC_ALL(start);
  // 发送设备地址
  __CH224_FUNC_ALL(send_addr, CH224_I2C_ADDR, 0); // 写操作
  // 等待ACK,检查 CH224 是否响应
  return __CH224_FUNC_ALL(wait_ack);
}
