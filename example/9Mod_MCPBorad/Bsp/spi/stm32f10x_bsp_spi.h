/**
 * @file stm32f10x_bsp_spi.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-02-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __STM32F10X_BSP_SPI_H__
#define __STM32F10X_BSP_SPI_H__

#define BSP_SPI_STATE_ERROR 0x04
#define BSP_SPI_STATE_BUSY 0x01
#define BSP_SPI_STATE_READY 0x00

unsigned char bsp_spi_init(void);
void bsp_spi_dc_reset(void);
void bsp_spi_dc_set(void);
void bsp_spi_cs_reset(void);
void bsp_spi_cs_set(void);
void bsp_spi_user_cs_reset(void);
void bsp_spi_user_cs_set(void);
unsigned char bsp_spi_get_state(void);
void bsp_spi_transmit(unsigned char data, unsigned int timeout);
void bsp_spi_receive(unsigned char *data);
#endif