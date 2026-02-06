/**
 * @file axk_gt20l16s.h
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef __AXK_GT20L16S_H__
#define __AXK_GT20L16S_H__

#if __has_include("stm32f10x_bsp_spi.h")
#include "stm32f10x_bsp_spi.h"
// 硬件抽象层宏定义
#define AXK_GT20L16S_ACLL(_func, ...) bsp_spi_##_func(__VA_ARGS__)

#else
// 如果没有找到底层驱动，定义为空或报错
#define AXK_GT20L16S_ACLL()
#ifdef AXK_GT20L16S_SPI
#define AXK_GT20L16S_SPI_BUSY
#define AXK_GT20L16S_SPI_READY
#endif
#error "AXK_GT20L16S_ACLL not found"

#endif
/**
 * @brief 从GT20L16S ROM中读取字体数据
 *
 * @param addrHigh 高地址字节
 * @param addrMid 中地址字节
 * @param addrLow 低地址字节
 * @param pbuff 存储读取数据的缓冲区
 * @param DataLen 要读取的数据长度
 */
void axk_gt20l16s_get_fonts_form_rom(unsigned char addrHigh,
                                     unsigned char addrMid,
                                     unsigned char addrLow,
                                     unsigned char *pbuff,
                                     unsigned char DataLen);
/**
 * @brief 将Unicode编码转换为GB2312字体地址
 *
 * @param unicode Unicode编码
 * @return unsigned int GB2312字体地址
 */
unsigned int unicode_to_gb2312_fontaddr(unsigned int unicode);
/**
 * @brief 判断字符串是否为UTF-8编码
 *
 * @param str 要判断的字符串
 * @param len 字符串长度
 * @return unsigned char 1: 是UTF-8编码, 0: 不是UTF-8编码
 */
unsigned char isStrUTF8(const char *str, unsigned short int len);
/**
 * @brief 将UTF-8编码的字符串转换为Unicode编码
 *
 * @param str UTF-8编码的字符串
 * @param byte_len 每个字符的字节长度
 * @return unsigned int Unicode编码
 */
unsigned int utf8_to_unicode(const char *str, unsigned char *byte_len);
#endif
