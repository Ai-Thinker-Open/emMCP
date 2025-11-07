#include "utf8_to_gb2312.h"

/**
 * @brief 判断字符串是否为UTF-8编码
 * 
 * @param str 
 * @param len 
 * @return uint8_t 
 */
uint8_t isStrUTF8(const char *str, uint16_t len){

  if (str == NULL || len < 1 || len > 4) {
        return 0; // 长度无效
    }

    switch (len) {
        case 1:
            // 1字节：0xxxxxxx
            return (str[0] & 0x80) == 0;
        case 2:
            // 2字节：110xxxxx 10xxxxxx
            return (str[0] & 0xE0) == 0xC0    // 首字节前3位为110
                && (str[1] & 0xC0) == 0x80;   // 后续字节前2位为10
        case 3:
            // 3字节：1110xxxx 10xxxxxx 10xxxxxx
            return (str[0] & 0xF0) == 0xE0    // 首字节前4位为1110
                && (str[1] & 0xC0) == 0x80    // 后续字节前2位为10
                && (str[2] & 0xC0) == 0x80;
        case 4:
            // 4字节：11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            return (str[0] & 0xF8) == 0xF0    // 首字节前5位为11110
                && (str[1] & 0xC0) == 0x80    // 后续字节前2位为10
                && (str[2] & 0xC0) == 0x80
                && (str[3] & 0xC0) == 0x80;
        default:
            return 0;
    }
} 
// --------------------------
// UTF-8解码为Unicode（标准实现）
// --------------------------
uint32_t utf8_to_unicode(const char *str, uint8_t *byte_len) {
    uint8_t c = (uint8_t)*str;
    if (c < 0x80) {  // 单字节ASCII
        *byte_len = 1;
        return c;
    } else if ((c & 0xE0) == 0xC0) {  // 双字节（110xxxxx）
        *byte_len = 2;
        return ((c & 0x1F) << 6) | (str[1] & 0x3F);
    } else if ((c & 0xF0) == 0xE0) {  // 三字节（1110xxxx）
        *byte_len = 3;
        return ((c & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else {  // 无效字符
        *byte_len = 1;
        return 0;
    }
}

// --------------------------
// Unicode转GB2312（精确映射表，覆盖常用字）
// 说明：从Unicode 0x4E00开始，前3755个汉字对应GB2312一级汉字
// --------------------------
uint32_t unicode_to_gb2312_fontaddr(uint32_t unicode) {
    uint32_t baseAddr, decodeAddr = 0;

    if(unicode <= 0x3017 && unicode >= 0x3000) baseAddr = 0x1d9e5;
    else if(unicode <= 0x9fa5 && unicode >= 0x4e00) baseAddr = 0x1bfbb;
    else if(unicode <= 0xfe6b && unicode >= 0xfe30) baseAddr = 0x16131;
    else if(unicode <= 0xff5e && unicode >= 0xff01) baseAddr = 0x1609c;
    else if(unicode <= 0xffe5 && unicode >= 0xffe0) baseAddr = 0x1601b;
    else baseAddr = 0;
    if(baseAddr != 0){
        decodeAddr = (unicode + baseAddr) * 2;
    }
    return decodeAddr;
}