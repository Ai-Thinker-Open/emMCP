/**
 * @file axk_ssd1306.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief SSD1306 OLED 驱动头文件
 * @version 0.2
 * @date 2026-02-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_SSD1306_H__
#define __AXK_SSD1306_H__

// 定义使用 SPI 接口
#define AXK_SSD1306_SPI

#if __has_include("stm32f10x_bsp_spi.h")
#include "stm32f10x_bsp_spi.h"
// 硬件抽象层宏定义
#define AXK_SSD1306_ACLL(_func, ...) bsp_spi_##_func(__VA_ARGS__)

#define AXK_SSD1306_SPI_READY BSP_SPI_STATE_READY
#define AXK_SSD1306_SPI_BUSY BSP_SPI_STATE_BUSY
#else
// 如果没有找到底层驱动，定义为空或报错
#define AXK_SSD1306_ACLL()
#ifdef AXK_SSD1306_SPI
#define AXK_SSD1306_SPI_BUSY
#define AXK_SSD1306_SPI_READY
#endif
#error "AXK_SSD1306_ACLL not found"

#endif

#if __has_include("axk_gt20l16s.h")
#include "axk_gt20l16s.h"
#define AXK_GT20L16_IS_AVAILABLE

#endif
// 命令/数据标志
#define AXK_SSD1306_WRITE_CMD 0
#define AXK_SSD1306_WRITE_DATA 1

typedef enum {
  FONT_SIEZE_8 = 0x08,
  FONT_SIEZE_12 = 0x0c,
  FONT_SIEZE_16 = 0x10,
  FONT_SIEZE_24 = 0x18,
} font_size_t;
/* ==================================================================
 * 函数声明
 * ================================================================== */

// 基础控制
/**
 * @brief 初始化 SSD1306 OLED 驱动
 *
 */
void axk_ssd1306_init(void);
/**
 * @brief 刷新 SSD1306 OLED 驱动显示
 *
 */
void axk_ssd1306_refresh(void);
/**
 * @brief 向 SSD1306 OLED 驱动写入一个字节数据
 *
 * @param data 要写入的数据
 * @param cmd 命令/数据标志（0：命令，1：数据）
 */
void axk_ssd1306_write_byte(unsigned char data, unsigned char cmd);
/**
 * @brief 设置 SSD1306 OLED 驱动的颜色模式
 *
 * @param i 颜色模式（0：正常，1：反色）
 */
void axk_ssd1306_set_color_turn(unsigned char i); // 反色显示
/**
 * @brief 设置 SSD1306 OLED 驱动的屏幕旋转角度
 *
 * @param i 旋转角度（0：0°，1：90°，2：180°，3：270°）
 */
void axk_ssd1306_set_display_turn(unsigned char i); // 屏幕旋转
/**
 * @brief 打开 SSD1306 OLED 驱动显示
 *
 */
void axk_ssd1306_open_display(void);
/**
 * @brief 关闭 SSD1306 OLED 驱动显示
 *
 */
void axk_ssd1306_close_display(void);
/**
 * @brief 清除屏幕内容
 * * 将所有显存写为 0
 */
void axk_ssd1306_clear_screen(void);
/**
 * @brief 设置写入数据的起始地址
 *
 * @param x 列地址（0-127）
 * @param y 页地址（0-7）
 */
void axk_ssd1306_set_address(unsigned char x, unsigned char y);
/**
 * @brief 绘制点
 *
 * @param x 点的 x 坐标
 * @param y 点的 y 坐标
 * @param draw_mode 绘制模式（0: 正常, 1: 反色）
 */
void axk_ssd1306_draw_point(unsigned char x, unsigned char y,
                            unsigned char draw_mode);
/**
 * @brief 绘制直线
 *
 * @param x1 直线起点列坐标 (0-127)
 * @param y1 直线起点页坐标 (0-63)
 * @param x2 直线终点列坐标 (0-127)
 * @param y2 直线终点页坐标 (0-63)
 * @param draw_mode 绘制模式 (0: 正常, 1: 反色)
 */
void axk_ssd1306_draw_line(unsigned char x1, unsigned char y1, unsigned char x2,
                           unsigned char y2, unsigned char draw_mode);
/**
 * @brief 绘制圆
 *
 * @param x0 圆心列坐标 (0-127)
 * @param y0 圆心页坐标 (0-63)
 * @param r 圆半径 (0-63)
 * @param draw_mode 绘制模式 (0: 正常, 1: 反色)
 */
void axk_ssd1306_draw_circle(unsigned char x0, unsigned char y0,
                             unsigned char r, unsigned char draw_mode);
/**
 * @brief 显示字符
 *
 * @param x 字符起点列坐标 (0-127)
 * @param y 字符起点页坐标 (0-63)
 * @param font_size 字体大小 (0: 6x8, 1: 8x16)
 * @param show_mode 显示模式 (0: 正常, 1: 反色)
 * @param c 要显示的字符
 */
void axk_ssd1306_show_char(unsigned char x, unsigned char y,
                           font_size_t font_size, unsigned char show_mode,
                           unsigned char c);
/**
 * @brief 显示 128x64 图像
 *
 * @param dp 图像数据指针
 */
void axk_ssd1306_show_image_128x64(unsigned char *dp);
/**
 * @brief 显示字符串
 *
 * @param x 字符串起点列坐标 (0-127)
 * @param y 字符串起点页坐标 (0-63)
 * @param font_size 字体大小 (0: 6x8, 1: 8x16)
 * @param show_mode 显示模式 (0: 正常, 1: 反色)
 * @param dp 要显示的字符串指针
 */
void axk_ssd1306_show_str(unsigned char x, unsigned char y,
                          font_size_t font_size, unsigned char show_mode,
                          unsigned char *str);
/**
 * @brief 显示数字
 *
 * @param x 数字起点列坐标 (0-127)
 * @param y 数字起点页坐标 (0-63)
 * @param font_size 字体大小 (0: 6x8, 1: 8x16)
 * @param show_mode 显示模式 (0: 正常, 1: 反色)
 * @param num 要显示的数字 (0-65535)
 */
void axk_ssd1306_show_numble(unsigned char x, unsigned char y,
                             font_size_t font_size, unsigned char show_mode,
                             unsigned int num);
#ifdef AXK_GT20L16_IS_AVAILABLE
void axk_ssd1306_show_gb2312_str(unsigned char x, unsigned char y, char *str);
void axk_ssd1306_show_utf8_str(unsigned char x, unsigned char y, const char *str);

#endif
#endif