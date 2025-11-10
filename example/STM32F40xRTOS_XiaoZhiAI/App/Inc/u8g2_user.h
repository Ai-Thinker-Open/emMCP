/**
 * @file u8g2_user.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-08
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef __U8G2_USER_H
#define __U8G2_USER_H

#include "u8g2.h"
#include "main.h"
#include "oled_ssd1306.h"
#include "utf8_to_gb2312.h"
#include "cozmo.h"
#include "u8g2_lable.h"
#include "boot_animation.h"

void u8g2_user_init(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);
#endif /* __U8G2_USER_H */