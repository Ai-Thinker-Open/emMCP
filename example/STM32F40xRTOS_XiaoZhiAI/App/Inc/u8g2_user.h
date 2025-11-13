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
#include "u8g2_gt20l16_font.h"
/**
 * @brief 表情枚举
 * 
 */
typedef enum {
  U8G2_EMOTION_NORMAL = 0x24,  // 平常冷漠脸
  U8G2_EMOTION_HAPPY = 0x20,   // 咧嘴笑
  U8G2_EMOTION_SMILE,          // 死亡微笑
  U8G2_EMOTION_SMILE_1,        // 微微笑
  U8G2_EMOTION_SMILE_2,        // 嘴角上扬
  U8G2_EMOTION_SAD = 0x25,     // 嘴角下扬
  U8G2_EMOTION_SAD_1,          // 难过
  U8G2_EMOTION_SAD_2,          // 沮丧
  U8G2_EMOTION_ANGE,           // 生气
  U8G2_EMOTION_DIE,            // 死亡
  U8G2_EMOTION_PLAYFUL = 0X30, // 调皮
  U8G2_EMOTION_SHAME,          // 害羞
  U8G2_EMOTION_PROUD,          // 得意
  U8G2_EMOTION_CONFUSED,       // 大聪明
  U8G2_EMOTION_EMBARRASSED,    // 冷漠大眼睛
  U8G2_EMOTION_SURPRISED,      // 惊讶
  U8G2_EMOTION_COOL,           // 眼镜耍酷
  U8G2_EMOTION_DELICIOUS,      // 咪眼裂嘴笑
  U8G2_EMOTION_MUSIC=0x4830,         // 音乐
}u8g2_emotion_t;
extern u8g2_emotion_t current_emotion;
void u8g2_user_init(u8g2_t *u8g2);
void draw(u8g2_t *u8g2);
#endif /* __U8G2_USER_H */