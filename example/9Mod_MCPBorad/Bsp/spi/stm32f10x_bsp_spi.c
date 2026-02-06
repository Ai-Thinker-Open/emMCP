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
#include "main.h"
#include "spi.h"
/**
 * @brief 初始化SPI1
 *
 * @return int 0:成功 -1:失败
 */
unsigned char bsp_spi_init(void) {
  MX_SPI1_Init();
  if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_RESET ||
      HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_ERROR) {
    return BSP_SPI_STATE_ERROR;
  }
  return BSP_SPI_STATE_READY;
}
/**
 * @brief 重置SPI1 DC引脚
 *
 */
void bsp_spi_dc_reset(void) {
  HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_RESET);
}
/**
 * @brief 设置SPI1 DC引脚
 *
 */
void bsp_spi_dc_set(void) {
  HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, GPIO_PIN_SET);
}
/**
 * @brief 重置SPI1 CS引脚
 *
 */
void bsp_spi_cs_reset(void) {
  HAL_GPIO_WritePin(OLED_CS1_GPIO_Port, OLED_CS1_Pin, GPIO_PIN_RESET);
}
/**
 * @brief 设置SPI1 CS引脚
 *
 */
void bsp_spi_cs_set(void) {
  HAL_GPIO_WritePin(OLED_CS1_GPIO_Port, OLED_CS1_Pin, GPIO_PIN_SET);
}
/**
 * @brief 重置用户CS引脚
 *
 */
void bsp_spi_user_cs_reset(void) {
  HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
}
/**
 * @brief 设置用户CS引脚
 *
 */
void bsp_spi_user_cs_set(void) {
  HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);
}
/**
 * @brief 获取SPI1状态
 *
 * @return unsigned char
 */
unsigned char bsp_spi_get_state(void) {
  if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_RESET ||
      HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_ERROR) {
    return BSP_SPI_STATE_ERROR;
  } else if (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY ||
             HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_RX ||
             HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX ||
             HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX_RX) {
    return BSP_SPI_STATE_BUSY;
  }
  return BSP_SPI_STATE_READY;
}
/**
 * @brief 发送SPI1数据
 *
 * @param data 要发送的数据
 * @param timeout 超时时间
 */
void bsp_spi_transmit(unsigned char data, unsigned int timeout) {
  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY)
    ;
  HAL_SPI_Transmit(&hspi1, &data, 1, timeout);
}
/**
 * @brief 接收SPI1数据
 *
 * @param data 接收数据的指针
 */
void bsp_spi_receive(unsigned char *data) {
  if (data == NULL) {
    return;
  }
  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY)
    ;
  HAL_SPI_Receive(&hspi1, data, 1, 100);
}