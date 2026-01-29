/**
 * @file stm32f10x_fw_i2c.h
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Ai-Thinker co.,ltd (c) 2026
 *
 */
#ifndef _STM32F10X_FW_I2C_H_
#define _STM32F10X_FW_I2C_H_

#include <stdint.h>

void stm32f10x_fw_i2c_gpio_init(void);
void stm32f10x_fw_i2c_init(void);
void stm32f10x_fw_i2c_start(void);
void stm32f10x_fw_i2c_stop(void);
void stm32f10x_fw_i2c_send_addr(uint8_t addr, uint8_t rw);
uint8_t stm32f10x_fw_i2c_wait_ack(void);

#endif