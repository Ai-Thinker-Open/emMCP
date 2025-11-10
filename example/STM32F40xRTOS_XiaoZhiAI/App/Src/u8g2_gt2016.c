/**
 * @file u8g2_gt2016.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-10
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "u8g2_gt2016.h"
#include "log.h"
#include "u8g2.h"

// 完全匹配你的OLED_Display_16x16绘制逻辑（16×16汉字）
static void u8g2_draw_16x16(u8g2_t *u8g2, uint8_t x, uint8_t y, const uint8_t *fontbuf) {
    // 假设你的OLED_Display_16x16按行绘制，每行2字节，共16行
    for (uint8_t row = 0; row < 16; row++) {
        // 每行2字节：左8像素和右8像素（与你的点阵存储一致）
        uint8_t left_byte = fontbuf[row * 2];   // 行内左侧8像素
        uint8_t right_byte = fontbuf[row * 2 + 1]; // 行内右侧8像素

        // 绘制左侧8像素
        for (uint8_t col = 0; col < 8; col++) {
            // 你的原始逻辑：bit7对应col0，bit0对应col7（从左到右）
            if (left_byte & (1 << (7 - col))) {
                u8g2_DrawPixel(u8g2, x + col, y + row);
            }
        }

        // 绘制右侧8像素
        for (uint8_t col = 0; col < 8; col++) {
            if (right_byte & (1 << (7 - col))) {
                u8g2_DrawPixel(u8g2, x + 8 + col, y + row);
            }
        }
    }
}

// 完全匹配你的OLED_Display_8x16绘制逻辑（8×16 ASCII）
static void u8g2_draw_8x16(u8g2_t *u8g2, uint8_t x, uint8_t y, const uint8_t *fontbuf) {
    // 假设你的OLED_Display_8x16按行绘制，每行1字节，共16行
    for (uint8_t row = 0; row < 16; row++) {
        uint8_t byte = fontbuf[row];  // 每行1字节（8像素）
        for (uint8_t col = 0; col < 8; col++) {
            // 你的原始逻辑：bit7对应col0，bit0对应col7
            if (byte & (1 << (7 - col))) {
                u8g2_DrawPixel(u8g2, x + col, y + row);
            }
        }
    }
}

// 主函数：复用你的GB2312字符串解析和地址计算逻辑
static void _u8g2_draw_gt20l16s(u8g2_t *u8g2, uint8_t x, uint8_t y, const char *gb2312_str) {
    uint8_t i = 0;
    uint8_t current_x = x;
    uint32_t fontaddr;  // 字库地址（与你的类型匹配）
    uint8_t addrHigh, addrMid, addrLow;
    uint8_t fontbuf[32];  // 存储16×16点阵（32字节）

    while (gb2312_str[i] != '\0') {
        // 处理第一区汉字（0xB0~0xF7 高字节，0xA1~0xFE 低字节）
        if ((gb2312_str[i] >= 0xB0 && gb2312_str[i] <= 0xF7) && 
            (gb2312_str[i+1] >= 0xA1)) {
            // 复用你的地址计算：(高字节-0xB0)*94 + (低字节-0xA1) + 846 → 乘以32
            fontaddr = (gb2312_str[i] - 0xB0) * 94;
            fontaddr += (gb2312_str[i+1] - 0xA1) + 846;
            fontaddr *= 32;

            // 解析地址高位、中位、低位
            addrHigh = (fontaddr & 0xFF0000) >> 16;
            addrMid = (fontaddr & 0xFF00) >> 8;
            addrLow = fontaddr & 0xFF;

            // 从GT20L16S读取32字节点阵
            OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 32);
            
            // 绘制16×16汉字
            u8g2_draw_16x16(u8g2, current_x, y, fontbuf);
            current_x += 16;  // 汉字宽度16像素
            i += 2;           // 跳过2字节GB2312编码
        }
        // 处理第二区汉字（0xA1~0xA3 高字节，0xA1~0xFE 低字节）
        else if ((gb2312_str[i] >= 0xA1 && gb2312_str[i] <= 0xA3) && 
                 (gb2312_str[i+1] >= 0xA1)) {
            // 复用你的地址计算：(高字节-0xA1)*94 + (低字节-0xA1) → 乘以32
            fontaddr = (gb2312_str[i] - 0xA1) * 94;
            fontaddr += (gb2312_str[i+1] - 0xA1);
            fontaddr *= 32;

            addrHigh = (fontaddr & 0xFF0000) >> 16;
            addrMid = (fontaddr & 0xFF00) >> 8;
            addrLow = fontaddr & 0xFF;

            OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 32);
            u8g2_draw_16x16(u8g2, current_x, y, fontbuf);
            current_x += 16;
            i += 2;
        }
        // 处理ASCII字符（0x20~0x7E）
        else if (gb2312_str[i] >= 0x20 && gb2312_str[i] <= 0x7E) {
            uint8_t ascii_buf[16];  // 存储8×16点阵（16字节）
            // 复用你的ASCII地址计算：(字符-0x20)*16 + 0x3CF80
            fontaddr = (gb2312_str[i] - 0x20) * 16 + 0x3CF80;

            addrHigh = (fontaddr & 0xFF0000) >> 16;
            addrMid = (fontaddr & 0xFF00) >> 8;
            addrLow = fontaddr & 0xFF;

            OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, ascii_buf, 16);
            u8g2_draw_8x16(u8g2, current_x, y, ascii_buf);
            current_x += 8;   // ASCII宽度8像素
            i += 1;           // 跳过1字节ASCII
        }
        // 跳过非法字符
        else {
            i++;
        }
    }
}

void u8g2_draw_gt20l16s(u8g2_t *u8g2, uint8_t x, uint8_t y, const char *gb2312_str) {
    uint8_t i = 0;
    uint8_t byte_len;
    uint8_t fontbuf[2];

    while (gb2312_str[i] != '\0') {

      if (isStrUTF8(&gb2312_str[i], 3)) {
         uint32_t unicode = utf8_to_unicode(&gb2312_str[i], &byte_len);
        if (unicode == 0) {
            i++;
            continue;
        }
        uint32_t fontaddr = unicode_to_gb2312_fontaddr(unicode);
        if (fontaddr == 0) {
            i += byte_len;
            continue;
        }
        OLED_get_data_from_ROM(fontaddr>>16&0XFF, fontaddr>>8&0XFF, fontaddr&0XFF, fontbuf, 2);
        _u8g2_draw_gt20l16s(u8g2,x, y, (char*)fontbuf);
        x += 16;
        i += byte_len;
      } else if ((gb2312_str[i] >= 0x20) && (gb2312_str[i] <= 0x7e)){
        unsigned char fontbuf[16];
        uint8_t addrHigh, addrMid, addrLow;
        uint32_t fontaddr ;
		fontaddr=(gb2312_str[i]-0x20);
		fontaddr=(unsigned long)(fontaddr*16);
		fontaddr=(unsigned long)(fontaddr+0x3cf80);
			
		addrHigh=(fontaddr&0xff0000)>>16;
		addrMid=(fontaddr&0xff00)>>8;
		addrLow=fontaddr&0xff;
			
		OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16);
		 u8g2_draw_8x16(u8g2, x, y, fontbuf);
        x += 8;
        i+=1;
	  }
    }
}
