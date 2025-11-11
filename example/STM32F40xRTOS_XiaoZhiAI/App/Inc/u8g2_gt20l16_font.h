/**
 * @file u8g2_gt20l16_font.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-11
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __U8G2_GT20L16_FONT_H__
#define __U8G2_GT20L16_FONT_H__

#include "u8g2.h"
#include "oled_ssd1306.h"

void u8g2_DrawGB2312(u8g2_t *u8g2, uint8_t x, uint8_t y, char *text);
void u8g2_DrawUTF8FromGT20l16(u8g2_t *u8g2, uint8_t x, uint8_t y, char *text);
#endif /* __U8G2_GT20L16_FONT_H__ */