#include "cozmo.h"
#include "stdbool.h"
// 适配128x64屏幕的眼睛参数（确保不超出屏幕边界）
#define EYE_CENTER_X 64    // 屏幕中心X（128/2=64）
#define EYE_CENTER_Y 32    // 屏幕中心Y（64/2=32）
#define EYE_SPACING 16     // 左右眼间距（缩小间距，避免超出屏幕）
#define EYE_BASE_W 10      // 基础眼宽（缩小尺寸，避免模糊）
#define EYE_BASE_H 8       // 基础眼高

// 绘制单只眼睛（确保坐标在屏幕范围内）
static void draw_eye(u8g2_t *u8g2, int x, int y, int w, int h, int round, bool fill) {
    // 限制眼睛尺寸，避免超出屏幕（128x64范围：X 0-127，Y 0-63）
    if (w < 0) w = 0;
    if (h < 0) h = 0;
    int x0 = x - w/2;
    int y0 = y - h/2;
    if (x0 < 0) x0 = 0;
    if (y0 < 0) y0 = 0;
    if (x0 + w > 127) w = 127 - x0;
    if (y0 + h > 63) h = 63 - y0;

    u8g2_SetDrawColor(u8g2, 1);
    if (fill) {
        u8g2_DrawRBox(u8g2, x0, y0, w, h, round); // 填充圆角矩形
    } else {
        u8g2_DrawRFrame(u8g2, x0, y0, w, h, round); // 空心圆角矩形
    }
}

void cozmo_init(Cozmo *cozmo, u8g2_t *u8g2, CozmoEmotion init_emotion) {
    cozmo->u8g2 = u8g2;
    cozmo->current_emotion = init_emotion;
    cozmo->target_emotion = init_emotion;
    cozmo->anim_frame = 10;
    cozmo->max_frame = 10;
}

void cozmo_set_emotion(Cozmo *cozmo, CozmoEmotion emotion) {
    if (cozmo->target_emotion != emotion) {
        cozmo->target_emotion = emotion;
        cozmo->anim_frame = 0;
    }
}

void cozmo_next_emotion(Cozmo *cozmo) {
    CozmoEmotion next = (cozmo->target_emotion + 1) % 7;
    cozmo_set_emotion(cozmo, next);
}

void cozmo_draw(Cozmo *cozmo) {
    u8g2_t *u8g2 = cozmo->u8g2;
    float progress = (float)cozmo->anim_frame / cozmo->max_frame;
    int left_x = EYE_CENTER_X - EYE_SPACING;  // 左眼X：64-16=48
    int right_x = EYE_CENTER_X + EYE_SPACING; // 右眼X：64+16=80
    int eye_y = EYE_CENTER_Y;                 // 眼睛Y：32

    // 确保进度在0-1范围内（避免负数或超过1）
    if (progress < 0) progress = 0;
    if (progress > 1) progress = 1;

    switch (cozmo->target_emotion) {
        case COZMO_HAPPY:
            // 开心：眯眼（宽10*0.8=8，高8*0.5=4，圆角4）
            draw_eye(u8g2, left_x, eye_y + 2, EYE_BASE_W*0.8*progress, EYE_BASE_H*0.5*progress, 4, true);
            draw_eye(u8g2, right_x, eye_y + 2, EYE_BASE_W*0.8*progress, EYE_BASE_H*0.5*progress, 4, true);
            break;
        case COZMO_ANGRY:
            // 愤怒：窄眼（宽10*0.7=7，高8*0.3=2.4，圆角1）
            draw_eye(u8g2, left_x, eye_y, EYE_BASE_W*0.7*progress, EYE_BASE_H*0.3*progress, 1, true);
            draw_eye(u8g2, right_x, eye_y, EYE_BASE_W*0.7*progress, EYE_BASE_H*0.3*progress, 1, true);
            break;
        case COZMO_SAD:
            // 难过：下垂眼（宽10*0.6=6，高8*0.4=3.2，圆角3）
            draw_eye(u8g2, left_x, eye_y + 3, EYE_BASE_W*0.6*progress, EYE_BASE_H*0.4*progress, 3, true);
            draw_eye(u8g2, right_x, eye_y + 3, EYE_BASE_W*0.6*progress, EYE_BASE_H*0.4*progress, 3, true);
            break;
        case COZMO_SURPRISED:
            // 惊讶：圆眼（宽10*1.2=12，高12，圆角6）
            draw_eye(u8g2, left_x, eye_y, EYE_BASE_W*1.2*progress, EYE_BASE_W*1.2*progress, 6, true);
            draw_eye(u8g2, right_x, eye_y, EYE_BASE_W*1.2*progress, EYE_BASE_W*1.2*progress, 6, true);
            break;
        case COZMO_SLEEPY:
            // 困倦：横线（长度12）
            int line_len = 12*progress;
            u8g2_SetDrawColor(u8g2, 1);
            u8g2_DrawLine(u8g2, left_x - line_len/2, eye_y, left_x + line_len/2, eye_y);
            u8g2_DrawLine(u8g2, right_x - line_len/2, eye_y, right_x + line_len/2, eye_y);
            break;
        case COZMO_PROUD:
            // 得意：斜眼（宽10*0.6=6，高8*0.5=4，圆角2）
            draw_eye(u8g2, left_x - 1, eye_y - 1, EYE_BASE_W*0.6*progress, EYE_BASE_H*0.5*progress, 2, true);
            draw_eye(u8g2, right_x + 1, eye_y - 1, EYE_BASE_W*0.6*progress, EYE_BASE_H*0.5*progress, 2, true);
            break;
        case COZMO_FOCUSED:
            // 专注：细长眼（宽10*1.0=10，高8*0.2=1.6，圆角2）
            draw_eye(u8g2, left_x, eye_y, EYE_BASE_W*1.0*progress, EYE_BASE_H*0.2*progress, 2, true);
            draw_eye(u8g2, right_x, eye_y, EYE_BASE_W*1.0*progress, EYE_BASE_H*0.2*progress, 2, true);
            break;
    }

    if (cozmo->anim_frame < cozmo->max_frame) {
        cozmo->anim_frame++;
    } else {
        cozmo->current_emotion = cozmo->target_emotion;
    }
}