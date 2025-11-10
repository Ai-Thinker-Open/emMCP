#ifndef COZMO_H
#define COZMO_H

#include "u8g2.h"
// Cozmo 原版表情枚举（无眉毛/嘴巴，纯眼睛表达）
typedef enum {
    COZMO_HAPPY,       // 开心：眯眼+上扬弧度
    COZMO_ANGRY,       // 愤怒：窄长眼+锐利边角
    COZMO_SAD,         // 难过：下垂眼+椭圆收缩
    COZMO_SURPRISED,   // 惊讶：大圆眼
    COZMO_SLEEPY,      // 困倦：细横线+缓慢闪烁
    COZMO_PROUD,       // 得意：斜眼+轻微眯起
    COZMO_FOCUSED      // 专注：细长眼+稳定显示
} CozmoEmotion;

// Cozmo 状态结构体
typedef struct {
    u8g2_t *u8g2;               // U8G2屏幕实例指针
    CozmoEmotion current_emotion; // 当前表情
    CozmoEmotion target_emotion;  // 目标表情（动画过渡）
    uint8_t anim_frame;          // 动画帧（0-10，10为静态）
    uint8_t max_frame;     // 最大动画帧数
} Cozmo;

// 初始化Cozmo表情控制器
void cozmo_init(Cozmo *cozmo, u8g2_t *u8g2, CozmoEmotion init_emotion);

// 切换到指定表情（带动画过渡）
void cozmo_set_emotion(Cozmo *cozmo, CozmoEmotion emotion);

// 循环切换下一个表情
void cozmo_next_emotion(Cozmo *cozmo);

// 绘制当前表情（主循环调用）
void cozmo_draw(Cozmo *cozmo);

#endif // COZMO_H