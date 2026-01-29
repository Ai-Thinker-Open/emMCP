/**
 * @file i2c_port.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-01-26
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef _I2C_PORT_H_
#define _I2C_PORT_H_
#include "stm32f1xx_hal.h"

#ifdef __STM32F1XX_H
#include "stm32f10x_fw_i2c.h"
#define CH224_I2C_SDA GPIO_PIN_6
#define CH224_I2C_SCL GPIO_PIN_7
#define __CH224_FUNC_ALL(_func, ...) stm32f10x_fw_i2c_##_func(__VA_ARGS__)

#pragma message "CH224_I2C: STM32F1xx HAL FW I2C port selected"

#elif defined(USER_CUSTOM_I2C_PORT)

#pragma message "CH224_I2C: USER_CUSTOM_I2C_PORT selected"
#else
#error "CH224_I2C: No valid I2C port selected"
#endif

#endif
