/**
 * @file oled_ssd1306.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __OLED_SSD1306_H
#define __OLED_SSD1306_H

#include "main.h"
#include "spi.h"
#include "utf8_to_gb2312.h"
//OLED DC引脚
#define OLED_DC_Clr()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_RESET)
#define OLED_DC_Set()  HAL_GPIO_WritePin(OLED_DC_GPIO_Port,OLED_DC_Pin,GPIO_PIN_SET)
//OLED CS1引脚
#define OLED_CS_Clr()  HAL_GPIO_WritePin(OLED_CS1_GPIO_Port,OLED_CS1_Pin,GPIO_PIN_RESET)
#define OLED_CS_Set()  HAL_GPIO_WritePin(OLED_CS1_GPIO_Port,OLED_CS1_Pin,GPIO_PIN_SET)
//OLED CS2引脚
#define OLED_ROM_CS_Clr()   HAL_GPIO_WritePin(OLED_CS2_GPIO_Port,OLED_CS2_Pin,GPIO_PIN_RESET)
#define OLED_ROM_CS_Set()   HAL_GPIO_WritePin(OLED_CS2_GPIO_Port, OLED_CS2_Pin, GPIO_PIN_SET)                                                 
  

#define OLED_CMD  0	//写命令
#define OLED_DATA 1 // 写数据

void OLED_ColorTurn(uint8_t i);
void OLED_DisplayTurn(uint8_t i);
void OLED_Clear(void);
void OLED_address(uint8_t x, uint8_t y);
void OLED_WR_Byte(uint8_t dat,uint8_t cmd);
void OLED_Display_128x64(uint8_t *dp);
void OLED_Display_16x16(uint8_t x,uint8_t y,uint8_t *dp);
void OLED_Display_8x16(uint8_t x,uint8_t y,uint8_t *dp);
void OLED_Display_5x7(uint8_t x,uint8_t y,uint8_t *dp);
void Send_Command_to_ROM(uint8_t dat);
uint8_t Get_data_from_ROM(void);
void OLED_get_data_from_ROM(uint8_t addrHigh,uint8_t addrMid,uint8_t addrLow,uint8_t *pbuff,uint8_t DataLen);
void OLED_Display_GB2312_string(uint8_t x, uint8_t y, char *text);
void OLED_Display_UTF8(uint8_t x, uint8_t y, const char *text);
void OLED_Display_string_5x7(uint8_t x,uint8_t y,uint8_t *text);
void OLED_ShowNum(uint8_t x, uint8_t y, float num, uint8_t len);

void OLED_Init(void);

#endif // !__OLED_SSD1306_H