/**
 * @file u8g2_lable.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "u8g2_lable.h"
#include "u8g2.h"

/**
 * 长文本滚动显示函数
 * @param u8g2: u8g2句柄
 * @param text: 需要滚动的文本
 * @param y_pos: 文本显示的Y坐标（行位置）
 * @param offset: 滚动偏移量（控制文本起始位置）
 */
void scroll_text(u8g2_t *u8g2, const char *text, uint8_t y_pos, int16_t offset) {

  // 选择支持中文的字体（确保字体包含所需字符集）
    u8g2_SetFont(u8g2, u8g2_font_wqy13_t_gb2312b); 
    
    uint16_t text_width = u8g2_GetUTF8Width(u8g2, text); // 文本总宽度
    uint16_t screen_width = u8g2_GetDisplayWidth(u8g2);  // 屏幕宽度
    uint16_t gap = screen_width / 3; // 文本间隔（避免首尾紧贴，可调整）
    
    // 文本宽度小于屏幕时无需滚动，直接居中显示
    if (text_width <= screen_width) {
        u8g2_DrawUTF8(u8g2, (screen_width - text_width) / 2, y_pos, text);
        u8g2_SendBuffer(u8g2);
        return;
    }
    
    // 计算总循环周期（文本宽度 + 间隔），用于偏移量取模
    int32_t cycle = text_width + gap;
    
    // 计算有效偏移量（通过取模实现无感知循环，避免直接重置）
    int16_t effective_offset = offset % cycle;
    // 处理负偏移的情况（确保取模后为正值）
    if (effective_offset < 0) {
        effective_offset += cycle;
    }
    // 转换为向左滚动的偏移（从屏幕右侧开始）
    effective_offset = screen_width - effective_offset;
    
    // 绘制主文本
    u8g2_DrawUTF8(u8g2, effective_offset, y_pos, text);
    
    // 绘制衔接文本（主文本左侧，距离主文本为文本宽度+间隔，实现无缝衔接）
    int16_t second_offset = effective_offset - (text_width + gap);
    u8g2_DrawUTF8(u8g2, second_offset, y_pos, text);
    
    
}