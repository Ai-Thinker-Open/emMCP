/**
 * @file stm32f10x_bsp_spi.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-02-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "stm32f10x_bsp_spi.h"

void bsp_spi_init(void) {}

void bsp_spi_dc_reset(void) {}
void bsp_spi_dc_set(void) {}

void bsp_spi_cs_reset(void) {}
void bsp_spi_cs_set(void) {}

unsigned char bsp_spi_get_state(void) { return HAL_SPI_STATE_READY; }
void bsp_spi_transmit(unsigned char data, unsigned int timeout) {}
