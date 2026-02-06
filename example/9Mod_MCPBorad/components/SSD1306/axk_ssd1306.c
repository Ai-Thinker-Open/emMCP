/**
 * @file axk_ssd1306.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief SSD1306 OLED 驱动源文件
 * @version 0.2
 * @date 2026-02-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_ssd1306.h"
#include "gt20l16s/axk_gt20l16s.h"
#include <stddef.h>
#include <sys/_intsup.h>
#include <sys/_types.h>
/**
 * @brief 包含 SSD1036 字体头文件
 *
 */
#if __has_include("axk_ssd1306_font.h")
#include "axk_ssd1306_font.h"
#else
#error "axk_ssd1306_font.h not found"
#endif
// 全局变量，用于字库地址计算
unsigned int axk_ssd1306_font_addr = 0;
unsigned char axk_oled_gram[144][8];
/**
 * @brief 向 SSD1306 写入一个字节
 *
 * @param data 要写入的数据或命令内容
 * @param cmd  AXK_SSD1306_WRITE_CMD (0) 表示写命令, AXK_SSD1306_WRITE_DATA (1)
 * 表示写数据
 */
void axk_ssd1306_write_byte(unsigned char data, unsigned char cmd) {
  if (cmd) {
#ifdef AXK_SSD1306_SPI
    AXK_SSD1306_ACLL(dc_set); // 写数据，DC 置高
#elif AXK_SSD1306_I2C
    // I2C 数据写入逻辑
#endif
  } else {
#ifdef AXK_SSD1306_SPI
    AXK_SSD1306_ACLL(dc_reset); // 写命令，DC 置低
#elif AXK_SSD1306_I2C
    // I2C 命令写入逻辑
#endif
  }

#ifdef AXK_SSD1306_SPI
  AXK_SSD1306_ACLL(cs_reset); // 拉低片选，开始传输
  // 发送数据 (长度1, 超时100)
  AXK_SSD1306_ACLL(transmit, data, 100);

  AXK_SSD1306_ACLL(cs_set); // 拉高片选，结束传输
  AXK_SSD1306_ACLL(dc_set); // 恢复 DC 状态
#elif AXK_SSD1306_I2C
  // I2C 传输逻辑
#endif
}

/**
 * @brief 设置反色显示
 *
 * @param i 1: 正常显示, 0: 反色显示
 */
void axk_ssd1306_set_color_turn(unsigned char i) {
  if (i == 0) {
#ifdef AXK_SSD1306_SPI
    axk_ssd1306_write_byte(0xA6, AXK_SSD1306_WRITE_CMD); // 正常显示
#elif AXK_SSD1306_I2C
    // I2C 逻辑
#endif
  } else if (i == 1) {
#ifdef AXK_SSD1306_SPI
    axk_ssd1306_write_byte(0xA7, AXK_SSD1306_WRITE_CMD); // 反色显示
#elif AXK_SSD1306_I2C
    // I2C 逻辑
#endif
  }
}

/**
 * @brief 设置屏幕旋转 180 度
 *
 * @param i 0: 正常方向, 1: 旋转 180 度
 */
void axk_ssd1306_set_display_turn(unsigned char i) {
  if (i == 0) {
#ifdef AXK_SSD1306_SPI
    axk_ssd1306_write_byte(0xC8, AXK_SSD1306_WRITE_CMD); // 正常扫描方向
    axk_ssd1306_write_byte(0xA1, AXK_SSD1306_WRITE_CMD);
#elif AXK_SSD1306_I2C
    // I2C 逻辑
#endif
  } else if (i == 1) {
#ifdef AXK_SSD1306_SPI
    axk_ssd1306_write_byte(0xC0, AXK_SSD1306_WRITE_CMD); // 反向扫描
    axk_ssd1306_write_byte(0xA0, AXK_SSD1306_WRITE_CMD);
#elif AXK_SSD1306_I2C
    // I2C 逻辑
#endif
  }
}

/**
 * @brief 开启 OLED 显示
 */
void axk_ssd1306_open_display(void) {
#ifdef AXK_SSD1306_SPI
  axk_ssd1306_write_byte(0x8D, AXK_SSD1306_WRITE_CMD); // 电荷泵设置
  axk_ssd1306_write_byte(0x14, AXK_SSD1306_WRITE_CMD); // 开启
  axk_ssd1306_write_byte(0xAF, AXK_SSD1306_WRITE_CMD); // 打开显示
#elif AXK_SSD1306_I2C
  // I2C 逻辑
#endif
}

/**
 * @brief 关闭 OLED 显示 (休眠模式)
 */
void axk_ssd1306_close_display(void) {
#ifdef AXK_SSD1306_SPI
  axk_ssd1306_write_byte(0x8D, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0x10, AXK_SSD1306_WRITE_CMD); // 关闭电荷泵
  axk_ssd1306_write_byte(0xAE, AXK_SSD1306_WRITE_CMD); // 关闭显示
#elif AXK_SSD1306_I2C
  // I2C 逻辑
#endif
}

/**
 * @brief 刷新 OLED 显示
 * * 将显存中的数据刷新到 OLED 屏幕上
 */
void axk_ssd1306_refresh(void) {
  unsigned char i, n;
  for (i = 0; i < 8; i++) {
    axk_ssd1306_write_byte(0xb0 + i, AXK_SSD1306_WRITE_CMD); // 设置行起始地址
    axk_ssd1306_write_byte(0x00, AXK_SSD1306_WRITE_CMD);     // 设置低列起始地址
    axk_ssd1306_write_byte(0x10, AXK_SSD1306_WRITE_CMD);     // 设置高列起始地址
    for (n = 0; n < 128; n++)
      axk_ssd1306_write_byte(axk_oled_gram[n][i], AXK_SSD1306_WRITE_DATA);
  }
}
/**
 * @brief 滚动显示
 *
 */
void axk_ssd1306_rolling() {}
/**
 * @brief 清除屏幕内容
 * * 将所有显存写为 0
 */
void axk_ssd1306_clear_screen(void) {
  unsigned char i, n;
  for (i = 0; i < 8; i++) {
    for (n = 0; n < 128; n++) {
      axk_oled_gram[n][i] = 0x00;
    }
  }
  axk_ssd1306_refresh();
}

/**
 * @brief 设置写入数据的起始地址
 *
 * @param x 列地址 (0-127)
 * @param y 页地址 (0-7)
 */
void axk_ssd1306_set_address(unsigned char x, unsigned char y) {
  axk_ssd1306_write_byte(0xb0 + y, AXK_SSD1306_WRITE_CMD); // 设置页地址
  axk_ssd1306_write_byte(((x & 0xf0) >> 4) | 0x10, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte((x & 0x0f), AXK_SSD1306_WRITE_CMD);
}
/**
 * @brief 绘制一个点
 *
 * @param x 点的列坐标 (0-127)
 * @param y 点的页坐标 (0-63)
 * @param draw_mode 绘制模式 (0: 清除, 1: 设置)
 */
void axk_ssd1306_draw_point(unsigned char x, unsigned char y,
                            unsigned char draw_mode) {
  unsigned char i, m, n;

  i = y / 8;
  m = y % 8;
  n = 1 << m;

  if (!draw_mode) {
    axk_oled_gram[x][i] |= n;
  } else {
    axk_oled_gram[x][i] = ~axk_oled_gram[x][i];
    axk_oled_gram[x][i] |= n;
    axk_oled_gram[x][i] = ~axk_oled_gram[x][i];
  }
}
/**
 * @brief 绘制一条直线
 *
 * @param x1 直线起点列坐标 (0-127)
 * @param y1 直线起点页坐标 (0-63)
 * @param x2 直线终点列坐标 (0-127)
 * @param y2 直线终点页坐标 (0-63)
 * @param draw_mode 绘制模式 (0: 清除, 1: 设置)
 */
void axk_ssd1306_draw_line(unsigned char x1, unsigned char y1, unsigned char x2,
                           unsigned char y2, unsigned char draw_mode) {

  unsigned int t;
  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
  int incx, incy, uRow, uCol;
  delta_x = x2 - x1; // 计算坐标增量
  delta_y = y2 - y1;
  uRow = x1; // 画线起点坐标
  uCol = y1;
  if (delta_x > 0)
    incx = 1; // 设置单步方向
  else if (delta_x == 0)
    incx = 0; // 垂直线
  else {
    incx = -1;
    delta_x = -delta_x;
  }
  if (delta_y > 0)
    incy = 1;
  else if (delta_y == 0)
    incy = 0; // 水平线
  else {
    incy = -1;
    delta_y = -delta_x;
  }
  if (delta_x > delta_y)
    distance = delta_x; // 选取基本增量坐标轴
  else
    distance = delta_y;
  for (t = 0; t < distance + 1; t++) {
    axk_ssd1306_draw_point(uRow, uCol, draw_mode); // 画点
    xerr += delta_x;
    yerr += delta_y;
    if (xerr > distance) {
      xerr -= distance;
      uRow += incx;
    }
    if (yerr > distance) {
      yerr -= distance;
      uCol += incy;
    }
  }
}
/**
 * @brief 绘制一个圆
 *
 * @param x0 圆心列坐标 (0-127)
 * @param y0 圆心页坐标 (0-63)
 * @param r 圆半径 (0-127)
 * @param draw_mode 绘制模式 (0: 反显, 1: 正常)
 */
void axk_ssd1306_draw_circle(unsigned char x0, unsigned char y0,
                             unsigned char r, unsigned char draw_mode) {
  if (x0 >= 128 || y0 >= 64 || (y0 + 2) > 8 || r >= 128) {
    return;
  }
  int a, b, num;
  a = 0;
  b = r;
  while (2 * b * b >= r * r) {
    axk_ssd1306_draw_point(x0 + a, y0 - b, draw_mode);
    axk_ssd1306_draw_point(x0 - a, y0 - b, draw_mode);
    axk_ssd1306_draw_point(x0 - a, y0 + b, draw_mode);
    axk_ssd1306_draw_point(x0 + a, y0 + b, draw_mode);

    axk_ssd1306_draw_point(x0 + b, y0 + a, draw_mode);
    axk_ssd1306_draw_point(x0 + b, y0 - a, draw_mode);
    axk_ssd1306_draw_point(x0 - b, y0 - a, draw_mode);
    axk_ssd1306_draw_point(x0 - b, y0 + a, draw_mode);
    a++;
    num = (a * a + b * b) - r * r; // 计算画的点离圆心的距离
    if (num > 0) {
      b--;
      a--;
    }
  }
}
/**
 * @brief 显示 128x64 的全屏点阵图像
 *
 * @param dp 图像数据指针
 */
void axk_ssd1306_show_image_128x64(unsigned char *dp) {
  unsigned char i, j;
  for (i = 0; i < 8; i++) {
    axk_ssd1306_set_address(0, i);
    for (j = 0; j < 128; j++) {
      axk_ssd1306_write_byte(*dp, AXK_SSD1306_WRITE_DATA);
      dp++;
    }
  }
}
/**
 * @brief 显示字符
 *
 * @param x 起始列 (0-127)
 * @param y 起始页 (0-7)
 * @param font_size 字体大小 (8/12/16/24)
 * @param show_mode 显示模式 (0: 正常, 1: 反色)
 * @param c 要显示的字符
 */
void axk_ssd1306_show_char(unsigned char x, unsigned char y,
                           font_size_t font_size, unsigned char show_mode,
                           unsigned char c) {
  unsigned char i, m, temp, size, chr1;
  unsigned char x0 = x, y0 = y;

  if (font_size == 8)
    size = 6;
  else
    size = (font_size / 8 + ((font_size % 8) ? 1 : 0)) * (font_size / 2);

  chr1 = c - ' '; // 计算偏移后的值
  for (i = 0; i < size; i++) {
    if (font_size == 8) {
      temp = axk_asc2_0806[chr1][i];
    } // 调用0806字体
    else if (font_size == 12) {
      temp = axk_asc2_1206[chr1][i];
    } // 调用1206字体
    else if (font_size == 16) {
      temp = axk_asc2_1608[chr1][i];
    } // 调用1608字体
    else if (font_size == 24) {
      temp = axk_asc2_2412[chr1][i];
    } // 调用2412字体
    else
      return;

    for (m = 0; m < 8; m++) {
      if (temp & 0x01)
        axk_ssd1306_draw_point(x, y, show_mode);
      else
        axk_ssd1306_draw_point(x, y, !show_mode);
      temp >>= 1;
      y++;
    }
    x++;
    if ((font_size != 8) && ((x - x0) == font_size / 2)) {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}
/**
 * @brief 显示字符串
 *
 * @param x 起始列 (0-127)
 * @param y 起始页 (0-7)
 * @param font_size 字体大小 (8/12/16/24)
 * @param show_mode 显示模式 (0: 正常, 1: 反色)
 * @param str 要显示的字符串
 */
void axk_ssd1306_show_str(unsigned char x, unsigned char y,
                          font_size_t font_size, unsigned char show_mode,
                          unsigned char *str) {

  while ((*str >= ' ') && (*str <= '~')) // 判断是不是非法字符!
  {
    axk_ssd1306_show_char(x, y, font_size, show_mode, *str);
    if (font_size == 8)
      x += 6;
    else
      x += font_size / 2;
    str++;
  }
}
static unsigned short int axk_ssd1306_pow(unsigned char m, unsigned char n) {
  unsigned short int result = 1;
  while (n--) {
    result *= m;
  }
  return result;
}

void axk_ssd1306_show_numble(unsigned char x, unsigned char y,
                             font_size_t font_size, unsigned char show_mode,
                             unsigned int num) {
  unsigned char t, temp, m = 0;
  if (font_size == 8)
    m = 2;
  for (t = 0; t < 4; t++) {
    temp = (num / axk_ssd1306_pow(10, 4 - t - 1)) % 10;
    if (temp == 0) {
      axk_ssd1306_show_char(x + (font_size / 2 + m) * t, y, font_size,
                            show_mode, '0');
    } else {
      axk_ssd1306_show_char(x + (font_size / 2 + m) * t, y, font_size,
                            show_mode, temp + '0');
    }
  }
}
/**
 * @brief 初始化 SSD1306 OLED 屏幕
 * * 发送一系列初始化命令，配置屏幕参数。
 */
void axk_ssd1306_init(void) {
  if (AXK_SSD1306_ACLL(init) != AXK_SSD1306_SPI_READY)
    return;
  axk_ssd1306_write_byte(0xAE, AXK_SSD1306_WRITE_CMD); // 关闭显示
  axk_ssd1306_write_byte(0x00, AXK_SSD1306_WRITE_CMD); // 设置低列地址
  axk_ssd1306_write_byte(0x10, AXK_SSD1306_WRITE_CMD); // 设置高列地址
  axk_ssd1306_write_byte(0x40, AXK_SSD1306_WRITE_CMD); // 设置起始行地址
  axk_ssd1306_write_byte(0x81, AXK_SSD1306_WRITE_CMD); // 对比度设置
  axk_ssd1306_write_byte(0xCF, AXK_SSD1306_WRITE_CMD); // 设置 SEG 输出电流
  axk_ssd1306_write_byte(0xA1, AXK_SSD1306_WRITE_CMD); // 设置段/列映射
  axk_ssd1306_write_byte(0xC8, AXK_SSD1306_WRITE_CMD); // 设置 COM/行扫描方向
  axk_ssd1306_write_byte(0xA6, AXK_SSD1306_WRITE_CMD); // 设置正常显示
  axk_ssd1306_write_byte(0xA8, AXK_SSD1306_WRITE_CMD); // 设置多路复用率 (1/64)
  axk_ssd1306_write_byte(0x3F, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0xD3, AXK_SSD1306_WRITE_CMD); // 设置显示偏移
  axk_ssd1306_write_byte(0x00, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0xD5, AXK_SSD1306_WRITE_CMD); // 设置显示时钟分频
  axk_ssd1306_write_byte(0x80, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0xD9, AXK_SSD1306_WRITE_CMD); // 设置预充电周期
  axk_ssd1306_write_byte(0xF1, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0xDA, AXK_SSD1306_WRITE_CMD); // 设置 COM 引脚硬件配置
  axk_ssd1306_write_byte(0x12, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0xDB, AXK_SSD1306_WRITE_CMD); // 设置 VCOMH
  axk_ssd1306_write_byte(0x40, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0x20, AXK_SSD1306_WRITE_CMD); // 设置页寻址模式
  axk_ssd1306_write_byte(0x02, AXK_SSD1306_WRITE_CMD);
  axk_ssd1306_write_byte(0x8D, AXK_SSD1306_WRITE_CMD); // 电荷泵设置
  axk_ssd1306_write_byte(0x14, AXK_SSD1306_WRITE_CMD); // 开启电荷泵
  axk_ssd1306_write_byte(0xA4, AXK_SSD1306_WRITE_CMD); // 全局显示开启
  axk_ssd1306_write_byte(0xA6, AXK_SSD1306_WRITE_CMD); // 关闭反色
  axk_ssd1306_clear_screen();                          // 清屏
  axk_ssd1306_write_byte(0xAF, AXK_SSD1306_WRITE_CMD); // 打开显示
}
/**
 * @brief 从字体芯片显示相关代码段
 */
#ifdef AXK_GT20L16_IS_AVAILABLE
unsigned int fontaddr = 0;
static void axk_ssd1306_display_data_form_font_chip(unsigned char x,
                                                    unsigned char y,
                                                    unsigned char *dp) {

  if (x >= 128 || y >= 64 || (y + 2) > 8) {
    return;
  }
  unsigned char i, j;
  unsigned char current_page = y;

  for (j = 0; j < 2; j++) {

    unsigned char start_col = x;
    unsigned char end_col = x + 15;
    if (end_col >= 128) {
      end_col = 128 - 1;
    }
    if (start_col > end_col) {
      current_page++;
      dp += 16;
      continue;
    }

    axk_ssd1306_write_byte(0xB0 + current_page, AXK_SSD1306_WRITE_CMD);
    axk_ssd1306_write_byte(0x10 | ((start_col >> 4) & 0x0F),
                           AXK_SSD1306_WRITE_CMD);
    axk_ssd1306_write_byte(0x00 | (start_col & 0x0F), AXK_SSD1306_WRITE_CMD);

    unsigned char draw_cols = end_col - start_col + 1;
    for (i = 0; i < draw_cols; i++) {
      axk_ssd1306_write_byte(dp[i], AXK_SSD1306_WRITE_DATA);
    }

    current_page++;
    dp += 16;
  }
}
static void axk_ssd1306_display_data_form_font_chip_8x16(unsigned char x,
                                                         unsigned char y,
                                                         unsigned char *dp) {
  unsigned char n, m;
  for (n = 0; n < 2; n++) {
    axk_ssd1306_set_address(x, y);
    for (m = 0; m < 8; m++) {
      axk_ssd1306_write_byte(*dp, AXK_SSD1306_WRITE_DATA);
      dp++;
    }
    y++;
  }
}
/**
 * @brief 显示GB2312字符串
 *
 * @param x 显示起始列
 * @param y 显示起始行
 * @param str 要显示的字符串
 */
void axk_ssd1306_show_gb2312_str(unsigned char x, unsigned char y, char *str) {
  unsigned char i = 0;
  unsigned char addrHigh, addrMid, addrLow;
  unsigned char fontbuf[32];
  while (str[i] > 0x00) {
    if ((str[i] >= 0xb0) && (str[i] <= 0xf7) && (str[i + 1] >= 0xa1)) {

      fontaddr = (str[i] - 0xb0) * 94;
      fontaddr += (str[i + 1] - 0xa1) + 846;
      fontaddr = fontaddr * 32;
      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = (fontaddr & 0xff);

      axk_gt20l16s_get_fonts_form_rom(addrHigh, addrMid, addrLow, fontbuf, 32);
      axk_ssd1306_display_data_form_font_chip(x, y, fontbuf);
      x += 16;
      i += 2;
    } else if ((str[i] >= 0xa1) && (str[i] <= 0xa3) && (str[i + 1] >= 0xa1)) {

      fontaddr = (str[i] - 0xa1) * 94;
      fontaddr += (str[i + 1] - 0xa1);
      fontaddr = fontaddr * 32;

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = (fontaddr & 0xff);

      axk_gt20l16s_get_fonts_form_rom(addrHigh, addrMid, addrLow, fontbuf, 32);
      axk_ssd1306_display_data_form_font_chip(x, y, fontbuf);
      x += 16;
      i += 2;
    } else if ((str[i] >= 0x20) && (str[i] <= 0x7e)) {

      unsigned char fontbuf[16];
      fontaddr = (str[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3cf80);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;

      axk_gt20l16s_get_fonts_form_rom(addrHigh, addrMid, addrLow, fontbuf, 16);
      axk_ssd1306_display_data_form_font_chip_8x16(x, y, fontbuf);

      x += 8;
      i += 1;
    } else
      i++;
  }
}
/**
 * @brief 显示UTF-8字符串
 *
 * @param x 显示起始列
 * @param y 显示起始行
 * @param str 要显示的字符串
 */
void axk_ssd1306_show_utf8_str(unsigned char x, unsigned char y,
                               const char *str) {
  unsigned char i = 0;
  unsigned char byte_len;
  static unsigned char fontbuf[2]; // 增加数组大小以容纳字符串结束符
  static unsigned char _fontbuf[16];
  while (str[i] != '\0') {

    // 先检查是否为单字节ASCII字符
    if ((str[i] >= 0x20) && (str[i] <= 0x7e)) {

      unsigned char addrHigh, addrMid, addrLow;
      unsigned int fontaddr;
      fontaddr = (str[i] - 0x20);
      fontaddr = (unsigned long)(fontaddr * 16);
      fontaddr = (unsigned long)(fontaddr + 0x3cf80);

      addrHigh = (fontaddr & 0xff0000) >> 16;
      addrMid = (fontaddr & 0xff00) >> 8;
      addrLow = fontaddr & 0xff;

      axk_gt20l16s_get_fonts_form_rom(addrHigh, addrMid, addrLow, _fontbuf, 16);
      axk_ssd1306_display_data_form_font_chip_8x16(x, y, _fontbuf);

      x += 8;
      i += 1;
    } else if (isStrUTF8(&str[i], 3)) {

      // 处理UTF-8字符
      unsigned int unicode = utf8_to_unicode(&str[i], &byte_len);
      if (unicode == 0) {
        i++;
        continue;
      }
      unsigned int fontaddr = unicode_to_gb2312_fontaddr(unicode);
      if (fontaddr == 0) {
        i += byte_len;
        continue;
      }
      axk_gt20l16s_get_fonts_form_rom(fontaddr >> 16 & 0XFF,
                                      fontaddr >> 8 & 0XFF, fontaddr & 0XFF,
                                      fontbuf, 2);
      axk_ssd1306_show_gb2312_str(x, y, (char *)fontbuf);
      x += 16;
      i += byte_len;
    } else {
      i++;
    }
  }
}
#endif