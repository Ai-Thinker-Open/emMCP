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
#include "axk_sht3x.h"
#include <stdint.h>
#include <sys/_intsup.h>
#include <sys/_types.h>
/**
 * @brief 初始化CH224 I2C
 *
 * @return unsigned char 0:成功 1:失败
 */
unsigned char axk_ch224_i2c_init(void) {

  // 初始化I2C外设
  AXK_CH224_I2C_ACLL(init);
  // 发送起始信号
  AXK_CH224_I2C_ACLL(start);
  // 发送设备地址
  AXK_CH224_I2C_ACLL(send_byte,
                     AXK_CH224_I2C_ADDR << 1 | AXK_CH224_WRITE_CMD); // 写操作
  // 等待ACK,检查 CH224 是否响应
  return AXK_CH224_I2C_ACLL(wait_ack);
}
