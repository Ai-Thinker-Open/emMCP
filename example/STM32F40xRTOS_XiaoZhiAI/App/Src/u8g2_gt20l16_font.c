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
#include <stddef.h>

void u8g2_Draw16X16(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t *font)
{
  u8g2_DrawXBMP(u8g2, x, y, 16, 16, font);
}

void u8g2_Draw8X16(u8g2_t *u8g2, uint8_t x, uint8_t y, uint8_t *font)
{
  u8g2_DrawXBMP(u8g2, x, y, 8, 16, font);
}


static uint32_t fontaddr=0;
void u8g2_DrawGB2312(u8g2_t *u8g2,uint8_t x,uint8_t y,char *text)
{
	uint8_t i=0;
	uint8_t addrHigh,addrMid,addrLow;
	uint8_t fontbuf[32];
	while(text[i]>0x00)
	{
		if((text[i]>=0xb0)&&(text[i]<=0xf7)&&(text[i+1]>=0xa1))
		{
			
			fontaddr=(text[i]-0xb0)*94;
			fontaddr+=(text[i+1]-0xa1)+846;
			fontaddr=fontaddr*32;
			addrHigh=(fontaddr&0xff0000)>>16;  
			addrMid=(fontaddr&0xff00)>>8;       
			addrLow=(fontaddr&0xff);            
			
			OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,32);
                        // OLED_Display_16x16(x,y,fontbuf);
            log_debug("原数据:");
            for (size_t i=0; i<32; i++) {
                log_printf("%02x ",fontbuf[i]);
            }
            log_printf("\r\n");
            uint8_t temp[32];
            for(int i=0; i<16; i++){  // 16行，每行2字节
            temp[i*2] = fontbuf[(15-i)*2];    // 反转行的第一个字节
            temp[i*2+1] = fontbuf[(15-i)*2+1];// 反转行的第二个字节
            }
            memcpy(fontbuf, temp, 32); // 覆盖原数据
            log_debug("反转后数据:");
             for (size_t i=0; i<32; i++) {
                log_printf("%02x ",fontbuf[i]);
            }
            log_printf("\r\n");
			u8g2_Draw16X16(u8g2,x,y,fontbuf);
			x+=16;
			i+=2;
    }
		else if((text[i]>=0xa1)&&(text[i]<=0xa3)&&(text[i+1]>=0xa1))
		{
			
			fontaddr=(text[i]-0xa1)*94;
			fontaddr+=(text[i+1]-0xa1);
			fontaddr=fontaddr*32;
			
			addrHigh=(fontaddr&0xff0000)>>16;
			addrMid=(fontaddr&0xff00)>>8;
			addrLow=(fontaddr&0xff);

                        OLED_get_data_from_ROM(addrHigh, addrMid, addrLow,
                                               fontbuf, 32);
            for (size_t i=0; i<32; i++) {
                log_printf("%02x ",fontbuf[i]);
            }
            log_printf("\r\n");
			u8g2_Draw16X16(u8g2,x,y,fontbuf);
			x+=16;
			i+=2;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {
			
			unsigned char fontbuf[16];
			fontaddr=(text[i]-0x20);
			fontaddr=(unsigned long)(fontaddr*16);
			fontaddr=(unsigned long)(fontaddr+0x3cf80);
			
			addrHigh=(fontaddr&0xff0000)>>16;
			addrMid=(fontaddr&0xff00)>>8;
			addrLow=fontaddr&0xff;
			
			OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16);
			u8g2_Draw8X16(u8g2,x,y,fontbuf);
			x+=8;
			i+=1;
    }
		else 
			i++;
  }
}

void u8g2_DrawUTF8FromGT20l16(u8g2_t *u8g2, uint8_t x, uint8_t y, char *text) {
   uint8_t i = 0;
    uint8_t byte_len;
    uint8_t fontbuf[2];

    while (text[i] != '\0') {

      if (isStrUTF8(&text[i], 3)) {
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
        uint8_t addrHigh, addrMid, addrLow;
        uint32_t fontaddr ;
		fontaddr=(text[i]-0x20);
		fontaddr=(unsigned long)(fontaddr*16);
		fontaddr=(unsigned long)(fontaddr+0x3cf80);
			
		addrHigh=(fontaddr&0xff0000)>>16;
		addrMid=(fontaddr&0xff00)>>8;
		addrLow=fontaddr&0xff;
			
		OLED_get_data_from_ROM(addrHigh,addrMid,addrLow,fontbuf,16);
		u8g2_Draw8X16(u8g2,x,y,fontbuf);
        x += 8;
        i+=1;
	  }
    }
}