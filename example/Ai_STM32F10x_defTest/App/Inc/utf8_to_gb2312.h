#ifndef UTF8_TO_GB2312_H
#define UTF8_TO_GB2312_H

#include "stm32f1xx_hal.h"

uint8_t isStrUTF8(const char *str, uint16_t len);
// UTF-8解码为Unicode
uint32_t utf8_to_unicode(const char *str, uint8_t *byte_len);
uint32_t unicode_to_gb2312_fontaddr(uint32_t unicode);


#endif