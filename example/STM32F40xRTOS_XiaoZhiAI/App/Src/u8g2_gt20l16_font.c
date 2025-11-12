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
#include "u8g2_gt20l16_font.h"
#include "log.h"
#include "u8g2.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#define T 0x80  // 明确8位掩码定义
static void u8g2_Draw16X16(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t *font)
{
  u8g2_DrawBitmap(u8g2, x, y, 2, 16, font);
}

static void u8g2_Draw8X16(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t *font)
{
 u8g2_DrawBitmap(u8g2, x, y, 1, 16, font);
}

// 行列式逆向取模 -> 逐行式顺向取模（16×16点阵）
static void reverseAndConvert_inverse32(unsigned char *output, unsigned char *input) {
    // 初始化输入数组为0
    for (int i = 0; i < 32; i++) {
        input[i] = 0;
    }

    // 处理前8个输出字节（对应原输入偶数索引字节0,2,4,...,14）
    for (int i = 0; i < 8; i++) {
        for (int b = 0; b < 8; b++) {
            unsigned char bit = (output[i] >> b) & 1;
            int input_index = 2 * b;
            int bit_position = 7 - i;
            if (bit) {
                input[input_index] |= (1 << bit_position);
            }
        }
    }

    // 处理output[8-15]（对应原输入奇数索引字节1,3,5,...,15）
    for (int i = 0; i < 8; i++) {
        for (int b = 0; b < 8; b++) {
            unsigned char bit = (output[8 + i] >> b) & 1;
            int input_index = 1 + 2 * b;
            int bit_position = 7 - i;
            if (bit) {
                input[input_index] |= (1 << bit_position);
            }
        }
    }

    // 处理output[16-23]（对应原输入偶数索引字节16,18,...,30）
    for (int i = 0; i < 8; i++) {
        for (int b = 0; b < 8; b++) {
            unsigned char bit = (output[16 + i] >> b) & 1;
            int input_index = 16 + 2 * b;
            int bit_position = 7 - i;
            if (bit) {
                input[input_index] |= (1 << bit_position);
            }
        }
    }

    // 处理output[24-31]（对应原输入奇数索引字节17,19,...,31）
    for (int i = 0; i < 8; i++) {
        for (int b = 0; b < 8; b++) {
            unsigned char bit = (output[24 + i] >> b) & 1;
            int input_index = 17 + 2 * b;
            int bit_position = 7 - i;
            if (bit) {
                input[input_index] |= (1 << bit_position);
            }
        }
    }
}

// 行列式逆向取模 -> 逐行式顺向取模（16×8或8×16点阵）
static void convertColumnToRow16(unsigned char *col_data, unsigned char *row_data) {
    // 初始化行数据
    for (int i = 0; i < 16; i++) {
        row_data[i] = 0;
    }
    
    // 行列式转逐行式
    for (int col = 0; col < 8; col++) {
        for (int row = 0; row < 16; row++) {
            unsigned char bit;
            if (row < 8) {
                bit = (col_data[col] >> row) & 1;  // 上半部分
            } else {
                bit = (col_data[col + 8] >> (row - 8)) & 1;  // 下半部分
            }
            // 设置到行数据中
            row_data[row] |= (bit << (7 - col));
        }
    }
}
static uint32_t fontaddr=0;
void u8g2_DrawGB2312(u8g2_t *u8g2,uint8_t x,uint8_t y,char *text)
{
	uint8_t addrHigh,addrMid,addrLow;
    uint8_t fontbuf[32];
    uint8_t i=0;
	
	if((text[i]>=0xb0)&&((text[i]<=0xf7)&&(text[i+1]>=0xa1))){		
		fontaddr=(text[i]-0xb0)*94;
		fontaddr+=(text[i+1]-0xa1)+846;
		fontaddr=fontaddr*32;
		addrHigh=(fontaddr&0xff0000)>>16;  
		addrMid=(fontaddr&0xff00)>>8;       
		addrLow=(fontaddr&0xff);            
		memset(fontbuf, 0, 32);
		OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32);
        uint8_t temp[32]={0};
        reverseAndConvert_inverse32(fontbuf,temp);
		u8g2_Draw16X16(u8g2,x,y,temp);
		x+=16;
    }
	else if((text[i]>=0xa1)&&(text[i]<=0xa3)&&(text[i+1]>=0xa1))
	{
			
        fontaddr=(text[i]-0xa1)*94;
        fontaddr+=(text[i+1]-0xa1);
        fontaddr=fontaddr*32;
            
        addrHigh=(fontaddr&0xff0000)>>16;
        addrMid=(fontaddr&0xff00)>>8;
        addrLow=(fontaddr&0xff);
        OLED_get_data_from_ROM(addrHigh, addrMid, addrLow,fontbuf, 32);
        uint8_t temp[32]={0};
        reverseAndConvert_inverse32(fontbuf,temp);
        u8g2_Draw16X16(u8g2,x,y,temp);
        x+=16;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {
			
        unsigned char fontbuf[16];
        fontaddr=(text[i]-0x20);
        fontaddr=(unsigned long)(fontaddr*16);
        fontaddr=(unsigned long)(fontaddr+0x3cf80);
        
        addrHigh=(fontaddr&0xff0000)>>16;
        addrMid=(fontaddr&0xff00)>>8;
        addrLow=fontaddr&0xff;
        OLED_get_data_from_ROM(addrHigh, addrMid, addrLow,fontbuf, 16);
        uint8_t temp[16]={0};
        convertColumnToRow16(fontbuf,temp);            
        u8g2_Draw8X16(u8g2,x,y,temp);
        x+=8;
    }
}

void u8g2_DrawUTF8FromGT20l16(u8g2_t *u8g2, uint8_t x, uint8_t y, char *text) {
   uint8_t i = 0;
    uint8_t byte_len;
  

    while (text[i] != '\0') {

      if (isStrUTF8(&text[i], 3)) {
          uint8_t fontbuf[2];
         uint32_t unicode = utf8_to_unicode(&text[i], &byte_len);
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
        // OLED_Display_GB2312_string(x, y, (char*)fontbuf);
        log_debug("0x%02x%02x",fontbuf[0],fontbuf[1]);
        u8g2_DrawGB2312(u8g2,x,y,(char*)fontbuf);
        x += 16;
        i += byte_len;
      } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)){
        unsigned char fontbuf[16];
        uint8_t temp[16]={0};
        uint32_t fontaddr ;
		fontaddr=(text[i]-0x20);
		fontaddr=(unsigned long)(fontaddr*16);
        fontaddr = (unsigned long)(fontaddr + 0x3cf80);
        memset(fontbuf, 0, 16);
        OLED_get_data_from_ROM(fontaddr>>16&0XFF, fontaddr>>8&0XFF, fontaddr&0XFF, fontbuf, 16);
        convertColumnToRow16(fontbuf, temp);
        u8g2_Draw8X16(u8g2,x,y,temp);
        
        x += 8;
        i+=1;
	  }
    }
}