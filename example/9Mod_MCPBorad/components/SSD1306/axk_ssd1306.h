/**
 * @file axk_ssd1306.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-02-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_SSD1306_H__
#define __AXK_SSD1306_H__

#define AXK_SSD1306_SPI

#if __has_include("stm32f10x_bsp_spi.h")
#include "stm32f10x_bsp_spi.h"
#define AXK_SSD1306_ACLL(_func, ...) bsp_spi_##_func(__VA_ARGS__)

#define AXK_SSD1306_SPI_BUSY HAL_SPI_STATE_BUSY
#define AXK_SSD1306_SPI_READY HAL_SPI_STATE_READY

#else
#define AXK_SSD1306_ACLL()
#ifdef AXK_SSD1306_SPI
#define AXK_SSD1306_SPI_BUSY
#define AXK_SSD1306_SPI_READY
#endif
#error "AXK_SSD1306_ACLL not found"

#endif

#define AXK_SSD1306_WRITE_CMD 0
#define AXK_SSD1306_READ_CMD 1

#endif