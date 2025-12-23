/**
 * @file oled_ssd1306.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-05
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "oled_ssd1306.h"
#include "log.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_spi.h"
#include "utf8_to_gb2312.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

// 反显函数
void OLED_ColorTurn(uint8_t i) {
  if (i == 0) {
    OLED_WR_Byte(0xA6, OLED_CMD); // 正常显示
  }
  if (i == 1) {
    OLED_WR_Byte(0xA7, OLED_CMD); // 反色显示
  }
}

// 屏幕旋转180�?
void OLED_DisplayTurn(uint8_t i) {
  if (i == 0) {

    OLED_WR_Byte(0xC8, OLED_CMD); // 正常显示
    OLED_WR_Byte(0xA1, OLED_CMD);
  }
  if (i == 1) {
    OLED_WR_Byte(0xC0, OLED_CMD); // 反转显示
    OLED_WR_Byte(0xA0, OLED_CMD);
    //   OLED_WR_Byte(cmd, sizeof(cmd), OLED_CMD);
  }
}

// 开启OLED显示
void OLED_DisPlay_On(void) {
  OLED_WR_Byte(0x8D, OLED_CMD); // ��ɱ�ʹ��?
  OLED_WR_Byte(0x14, OLED_CMD); // ������ɱ�?
  OLED_WR_Byte(0xAF, OLED_CMD); // ������Ļ
}

// 关闭OLED显示
void OLED_DisPlay_Off(void) {
  OLED_WR_Byte(0x8D, OLED_CMD);
  OLED_WR_Byte(0x10, OLED_CMD);
  OLED_WR_Byte(0xAE, OLED_CMD);
}

// 向SSD1306写入一�?字节�?
// mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(uint8_t dat, uint8_t cmd) {
  if (cmd)
    OLED_DC_Set();
  else
    OLED_DC_Clr();
  OLED_CS_Clr();

  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY)
    ;
  HAL_SPI_Transmit(&hspi1, &dat, 1, 100);
  OLED_CS_Set();
  OLED_DC_Set();
}

// 清屏函数
void OLED_Clear(void) {
  uint8_t i, n;
  for (i = 0; i < 8; i++) {
    OLED_WR_Byte(0xb0 + i, OLED_CMD);
    OLED_WR_Byte(0x10, OLED_CMD);
    OLED_WR_Byte(0x00, OLED_CMD);
    for (n = 0; n < 128; n++) {
      OLED_WR_Byte(0x00, OLED_DATA);
    }
  }
}
// 设置起�?�地址
void OLED_address(uint8_t x, uint8_t y) {

  OLED_WR_Byte(0xb0 + y, OLED_CMD);                 // 设置页地址
  OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD); // 设置列地址的高4�?
  OLED_WR_Byte((x & 0x0f), OLED_CMD); // 设置列地址的低4�?
}

// 显示128x64点阵图像
void OLED_Display_128x64(uint8_t *dp) {
  uint8_t i, j;

  for (i = 0; i < 8; i++) {
    OLED_address(0, i);
    for (j = 0; j < 128; j++) {
      OLED_WR_Byte(
          *dp, OLED_DATA); // 写数�?到OLED,每写完一�?8位的数据后列地址�?动加1
      dp++;
    }
  }
}

void OLED_Display_16x16(uint8_t x, uint8_t y, uint8_t *dp) {

  if (x >= 128 || y >= 64 || (y + 2) > 8) {
    return;
  }
  uint8_t i, j;
  uint8_t current_page = y;

  for (j = 0; j < 2; j++) {

    uint8_t start_col = x;
    uint8_t end_col = x + 15;
    if (end_col >= 128) {
      end_col = 128 - 1;
    }
    if (start_col > end_col) {
      current_page++;
      dp += 16;
      continue;
    }

    OLED_WR_Byte(0xB0 + current_page, OLED_CMD);
    OLED_WR_Byte(0x10 | ((start_col >> 4) & 0x0F), OLED_CMD);
    OLED_WR_Byte(0x00 | (start_col & 0x0F), OLED_CMD);

    uint8_t draw_cols = end_col - start_col + 1;
    for (i = 0; i < draw_cols; i++) {
      OLED_WR_Byte(dp[i], OLED_DATA);
    }

    current_page++;
    dp += 16;
  }
}
// 显示8x16点阵图像、ASCII, �?8x16点阵的自造字符、其他图�?
void OLED_Display_8x16(uint8_t x, uint8_t y, uint8_t *dp) {
  uint8_t i, j;
  for (j = 0; j < 2; j++) {
    OLED_address(x, y);
    for (i = 0; i < 8; i++) {
      OLED_WR_Byte(*dp,
                   OLED_DATA); // 写数�?到LCD,每写完一�?8位的数据后列地址�?动加1
      dp++;
    }
    y++;
  }
}

// 显示5*7点阵图像、ASCII, �?5x7点阵的自造字符、其他图�?
void OLED_Display_5x7(uint8_t x, uint8_t y, uint8_t *dp) {
  uint8_t i;
  OLED_address(x, y);
  for (i = 0; i < 6; i++) {
    OLED_WR_Byte(*dp, OLED_DATA);
    dp++;
  }
}

// 送指令到晶联�?字库IC
void Send_Command_to_ROM(uint8_t dat) {
  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY)
    ;
  HAL_SPI_Transmit(&hspi1, &dat, 1, 100);
}

// 从晶联�??字库IC�?取汉字或字�?�数�?�?1�?字节�?
uint8_t Get_data_from_ROM(void) {
  uint8_t read = 0;
  while (HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY)
    ;
  HAL_SPI_Receive(&hspi1, &read, 1, 100);
  return read;
}

void OLED_get_data_from_ROM(uint8_t addrHigh, uint8_t addrMid, uint8_t addrLow,
                            uint8_t *pbuff, uint8_t DataLen) {
  uint8_t i;
  if (pbuff == NULL)
    return;
  uint8_t *p = pbuff;
  OLED_ROM_CS_Clr();
  Send_Command_to_ROM(0x03);
  Send_Command_to_ROM(addrHigh);
  Send_Command_to_ROM(addrMid);
  Send_Command_to_ROM(addrLow);
  for (i = 0; i < DataLen; i++) {
    *(p + i) = Get_data_from_ROM();
  }
  OLED_ROM_CS_Set();
}

uint32_t fontaddr = 0;
void OLED_Display_GB2312_string(uint8_t x, uint8_t y, char *text) {
  uint8_t i = 0;
  uint8_t addrHigh, addrMid, addrLow;
  uint8_t fontbuf[32];
  while (text[i] > 0x00) {
    if ((text[i] >= 0xb0) && (text[i] <= 0xf7) && (text[i + 1] >= 0xa1)) {

      fontaddr = (text[i] - 0xb0) * 94;
      fontaddr += (text[i + 1] - 0xa1) + 846;
      fontaddr = fontaddr * 32;
      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = (fontaddr & 0xff);

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 32);
      OLED_Display_16x16(x, y, fontbuf);
      x += 16;
      i += 2;
    } else if ((text[i] >= 0xa1) && (text[i] <= 0xa3) &&
               (text[i + 1] >= 0xa1)) {

      fontaddr = (text[i] - 0xa1) * 94;
      fontaddr += (text[i + 1] - 0xa1);
      fontaddr = fontaddr * 32;

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = (fontaddr & 0xff);

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 32);
      OLED_Display_16x16(x, y, fontbuf);
      x += 16;
      i += 2;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {

      unsigned char fontbuf[16];
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3cf80);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 16);
      OLED_Display_8x16(x, y, fontbuf);
      x += 8;
      i += 1;
    } else
      i++;
  }
}
// OLED��ʾUTF-8�ַ���
void OLED_Display_UTF8(uint8_t x, uint8_t y, const char *text) {
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
      OLED_get_data_from_ROM(fontaddr >> 16 & 0XFF, fontaddr >> 8 & 0XFF,
                             fontaddr & 0XFF, fontbuf, 2);
      OLED_Display_GB2312_string(x, y, (char *)fontbuf);
      x += 16;
      i += byte_len;
    } else if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {
      unsigned char fontbuf[16];
      uint8_t addrHigh, addrMid, addrLow;
      uint32_t fontaddr;
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3cf80);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 16);
      OLED_Display_8x16(x, y, fontbuf);
      x += 8;
      i += 1;
    }
  }
}
void OLED_Display_string_5x7(uint8_t x, uint8_t y, uint8_t *text) {
  uint8_t i = 0;
  uint8_t addrHigh, addrMid, addrLow;
  while (text[i] > 0x00) {
    if ((text[i] >= 0x20) && (text[i] <= 0x7e)) {
      uint8_t fontbuf[8];
      fontaddr = (text[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 8);
      fontaddr = (unsigned long)(fontaddr + 0x3bfc0);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;

      OLED_get_data_from_ROM(addrHigh, addrMid, addrLow, fontbuf, 8);
      OLED_Display_5x7(x, y, fontbuf);
      x += 6;
      i += 1;
    } else
      i++;
  }
}

// 显示2�?数字
// x,y :起点坐标
// num1：�?�显示的小数
// len :数字的位�?
void OLED_ShowNum(uint8_t x, uint8_t y, float num1, uint8_t len) {
  uint8_t i;
  uint32_t t, num;
  x = x + len * 8 + 8; // 要显示的小数最低位的横坐标
  num = num1 * 100;    // 将小数左移两位并�?化为整数
  OLED_Display_GB2312_string(x - 24, y, "."); // 显示小数�?
  for (i = 0; i < len; i++) {
    t = num % 10;   // 取个位数的数�?
    num = num / 10; // 将整数右移一�?
    x -= 8;
    if (i == 2) {
      x -= 8;
    } // 当显示出来两�?小数之后，空出小数点的位�?
    switch (t) {
    case 0:
      OLED_Display_GB2312_string(x, y, "0");
      break;
    case 1:
      OLED_Display_GB2312_string(x, y, "1");
      break;
    case 2:
      OLED_Display_GB2312_string(x, y, "2");
      break;
    case 3:
      OLED_Display_GB2312_string(x, y, "3");
      break;
    case 4:
      OLED_Display_GB2312_string(x, y, "4");
      break;
    case 5:
      OLED_Display_GB2312_string(x, y, "5");
      break;
    case 6:
      OLED_Display_GB2312_string(x, y, "6");
      break;
    case 7:
      OLED_Display_GB2312_string(x, y, "7");
      break;
    case 8:
      OLED_Display_GB2312_string(x, y, "8");
      break;
    case 9:
      OLED_Display_GB2312_string(x, y, "9");
      break;
    }
  }
}

// OLED的初始化
void OLED_Init(void) {

  OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
  OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
  OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
  OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM
  OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
  OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current
  OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping
  OLED_WR_Byte(0xC8, OLED_CMD); // Set COM/Row Scan
  OLED_WR_Byte(0xA6, OLED_CMD); //--set normal
  OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f, OLED_CMD); //--1/64 duty
  OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM
  OLED_WR_Byte(0x00, OLED_CMD); //-not offset
  OLED_WR_Byte(0xd5, OLED_CMD); //--set display clock divide ratio/oscillator
  OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as
  OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
  OLED_WR_Byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1
  OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
  OLED_WR_Byte(0x12, OLED_CMD);
  OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
  OLED_WR_Byte(0x40, OLED_CMD); // Set VCOM Deselect Level
  OLED_WR_Byte(0x20, OLED_CMD); //-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02, OLED_CMD); //
  OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
  OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
  OLED_WR_Byte(0xA4, OLED_CMD); // Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6, OLED_CMD); // Disable Inverse Display On (0xa6/a7)
  OLED_Clear();
  OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
}
