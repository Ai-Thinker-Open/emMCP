/**
 * @file axk_gt20l16s.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-02-06
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_gt20l16s.h"
#include <string.h>
#include <sys/_intsup.h>
#include <sys/_types.h>
/**
 * @brief 向GT20L16S写入命令
 *
 * @param cmd 要写入的命令
 */
static void axk_gt20l16s_write_cmd(unsigned char cmd) {
  AXK_GT20L16S_ACLL(transmit, cmd, 100);
}
/**
 * @brief 读取GT20L16S的ROM数据
 *
 * @return unsigned char 读取到的ROM数据
 */
static unsigned char axk_gt20l16s_read_rom(void) {
  unsigned char rom_data = 0;
  AXK_GT20L16S_ACLL(receive, &rom_data);
  return rom_data;
}
/**
 * @brief 从GT20L16S的ROM中读取字体数据
 *
 * @param addrHigh 高地址字节
 * @param addrMid 中地址字节
 * @param addrLow 低地址字节
 * @param pbuff 存储字体数据的缓冲区
 * @param DataLen 要读取的字体数据长度
 */
void axk_gt20l16s_get_fonts_form_rom(unsigned char addrHigh,
                                     unsigned char addrMid,
                                     unsigned char addrLow,
                                     unsigned char *pbuff,
                                     unsigned char DataLen) {
  unsigned char i;
  if (pbuff == NULL)
    return;
  unsigned char *p = pbuff;
  AXK_GT20L16S_ACLL(user_cs_reset);
  axk_gt20l16s_write_cmd(0x03);
  axk_gt20l16s_write_cmd(addrHigh);
  axk_gt20l16s_write_cmd(addrMid);
  axk_gt20l16s_write_cmd(addrLow);

  for (i = 0; i < DataLen; i++) {
    *(p + i) = axk_gt20l16s_read_rom();
  }
  AXK_GT20L16S_ACLL(user_cs_set);
}

unsigned unicode_to_gb2312_fontaddr(unsigned int unicode) {
  unsigned int baseAddr, decodeAddr = 0;

  if (unicode <= 0x3017 && unicode >= 0x3000)
    baseAddr = 0x1d9e5;
  else if (unicode <= 0x9fa5 && unicode >= 0x4e00)
    baseAddr = 0x1bfbb;
  else if (unicode <= 0xfe6b && unicode >= 0xfe30)
    baseAddr = 0x16131;
  else if (unicode <= 0xff5e && unicode >= 0xff01)
    baseAddr = 0x1609c;
  else if (unicode <= 0xffe5 && unicode >= 0xffe0)
    baseAddr = 0x1601b;
  else
    baseAddr = 0;
  if (baseAddr != 0) {
    decodeAddr = (unicode + baseAddr) * 2;
  }
  return decodeAddr;
}
unsigned int utf8_to_unicode(const char *str, unsigned char *byte_len) {
  unsigned char c = (unsigned char)*str;
  if (c < 0x80) { // 单字节ASCII
    *byte_len = 1;
    return c;
  } else if ((c & 0xE0) == 0xC0) { // 双字节（110xxxxx）
    *byte_len = 2;
    return ((c & 0x1F) << 6) | (str[1] & 0x3F);
  } else if ((c & 0xF0) == 0xE0) { // 三字节（1110xxxx）
    *byte_len = 3;
    return ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
  } else { // 无效字符
    *byte_len = 1;
    return 0;
  }
}
unsigned char isStrUTF8(const char *str, unsigned short int len) {

  if (str == NULL || len < 1 || len > 4) {
    return 0; // 长度无效
  }

  switch (len) {
  case 1:
    // 1字节：0xxxxxxx
    return (str[0] & 0x80) == 0;
  case 2:
    // 2字节：110xxxxx 10xxxxxx
    return (str[0] & 0xE0) == 0xC0     // 首字节前3位为110
           && (str[1] & 0xC0) == 0x80; // 后续字节前2位为10
  case 3:
    // 3字节：1110xxxx 10xxxxxx 10xxxxxx
    return (str[0] & 0xF0) == 0xE0    // 首字节前4位为1110
           && (str[1] & 0xC0) == 0x80 // 后续字节前2位为10
           && (str[2] & 0xC0) == 0x80;
  case 4:
    // 4字节：11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    return (str[0] & 0xF8) == 0xF0    // 首字节前5位为11110
           && (str[1] & 0xC0) == 0x80 // 后续字节前2位为10
           && (str[2] & 0xC0) == 0x80 && (str[3] & 0xC0) == 0x80;
  default:
    return 0;
  }
}