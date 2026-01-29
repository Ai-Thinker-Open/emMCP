/**
 * @file ch224_i2c.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _CH224_I2C_H_
#define _CH224_I2C_H_

#if __has_include("stm32f10x_bsp_i2c.h")
#include "stm32f10x_bsp_i2c.h"
#define AXK_CH224_DELAY_MS(x) delay_ms(x)
#define AXK_CH224_I2C_ACLL(_func, ...) bsp_i2c_##_func(__VA_ARGS__)
#pragma message "stm32f10x_bsp_i2c.h included for CH224 I2C operations"
#else
#error "Please include the appropriate I2C header for CH224."
#endif

#define AXK_CH224_I2C_ADDR 0x22
#define AXK_CH224_WRITE_CMD 0
#define AXK_CH224_READ_CMD 1

#define AXK_CH224_ACK 0
#define AXK_CH224_NACK 1
#endif
