/**
 * @file u8g2_lable.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __U8G2_LABLE_H
#define __U8G2_LABLE_H
#include "u8g2.h"
#include "main.h"

void scroll_text(u8g2_t *u8g2, const char *text, uint8_t y_pos, int16_t offset);

#endif // !__U8G2_LABLE_H