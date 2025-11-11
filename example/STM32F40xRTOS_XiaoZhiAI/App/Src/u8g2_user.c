/**
 * @file u8g2_user.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "u8g2_user.h"
#include "main.h"
#include "spi.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "u8g2.h"
#include "u8x8.h"
#include <stdint.h>

void draw(u8g2_t *u8g2)
{
    u8g2_SetFontMode(u8g2, 1); /*字体模式选择*/
    u8g2_SetFontDirection(u8g2, 0); /*字体方向选择*/
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf); /*字库选择*/
    u8g2_DrawStr(u8g2, 0, 20, "U");

    u8g2_SetFontDirection(u8g2, 1);
    u8g2_SetFont(u8g2, u8g2_font_inb30_mn);
    u8g2_DrawStr(u8g2, 21,8,"8");

    u8g2_SetFontDirection(u8g2, 0);
    u8g2_SetFont(u8g2, u8g2_font_inb24_mf);
    u8g2_DrawStr(u8g2, 51,30,"g");
    u8g2_DrawStr(u8g2, 67,30,"\xb2");

    u8g2_DrawHLine(u8g2, 2, 35, 47);
    u8g2_DrawHLine(u8g2, 3, 36, 47);
    u8g2_DrawVLine(u8g2, 45, 32, 12);
    u8g2_DrawVLine(u8g2, 46, 33, 12);

    u8g2_SetFont(u8g2, u8g2_font_4x6_tr);
    u8g2_DrawStr(u8g2, 1,54,"github.com/olikraus/u8g2");
}
/**
 * @brief u8x8 4wire spi 回调函数
 * 
 * @param u8x8 
 * @param msg 
 * @param arg_int 
 * @param arg_ptr 
 * @return uint8_t 
 */
static uint8_t u8x8_bute_4wire_hw_spi(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int,void *arg_ptr)
{
  switch (msg) {
  case U8X8_MSG_BYTE_SEND:
    HAL_SPI_Transmit(&hspi1, (uint8_t *)arg_ptr, arg_int,100);
    
      break;
  case U8X8_MSG_BYTE_INIT:
    break;
  case U8X8_MSG_BYTE_SET_DC:
    HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int );
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_enable_level);
      u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO, u8x8->display_info->post_chip_enable_wait_ns, NULL);
  break;
  case U8X8_MSG_BYTE_END_TRANSFER:
      u8x8->gpio_and_delay_cb(u8x8, U8X8_MSG_DELAY_NANO,u8x8->display_info->pre_chip_disable_wait_ns,NULL);
      u8x8_gpio_SetCS(u8x8, u8x8->display_info->chip_disable_level);
  break;
  default:
    return 0;
  }
  return 1 ;
}
/**
 * @brief u8x8 gpio delay 回调函数
 * 
 * @param u8x8 
 * @param msg 
 * @param arg_int 
 * @param arg_ptr 
 * @return uint8_t 
 */
static uint8_t u8x8_gpio_and_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch (msg) {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      break;
    case U8X8_MSG_DELAY_MILLI:
      HAL_Delay(arg_int);
      break;
    case U8X8_MSG_GPIO_CS:
      HAL_GPIO_WritePin(OLED_CS1_GPIO_Port, OLED_CS1_Pin, arg_int );
      break;
    case U8X8_MSG_GPIO_DC:
      HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int );
      break;
    default:
      return 0;
    }
    return 1 ;
}
/**
 * @brief u8g2 初始化
 * 
 * @param u8g2 
 */
void u8g2_user_init(u8g2_t *u8g2)
{
  u8g2_Setup_ssd1306_128x64_noname_f(u8g2, U8G2_R0, u8x8_bute_4wire_hw_spi, u8x8_gpio_and_delay);
  u8g2_InitDisplay(u8g2);
  u8g2_SetPowerSave(u8g2, 0);
  u8g2_ClearBuffer(u8g2);
}

