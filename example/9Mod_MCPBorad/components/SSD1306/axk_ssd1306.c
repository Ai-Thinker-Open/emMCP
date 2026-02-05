/**
 * @file axk_ssd1306.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2026-02-05
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "axk_ssd1306.h"

void axk_ssd1306_init(void) {}

void axk_ssd1306_write_byte(unsigned char data, unsigned char cmd) {
  if (cmd) {
#ifdef AXK_SSD1306_SPI
    AXK_SSD1306_ACLL(dc_set);
#endif
  } else {
#ifdef AXK_SSD1306_SPI
    AXK_SSD1306_ACLL(dc_reset);
#endif
  }
#ifdef AXK_SSD1306_SPI
  AXK_SSD1306_ACLL(cs_reset);
  while (AXK_SSD1306_ACLL(get_state) == AXK_SSD1306_SPI_READY)
    ;
  AXK_SSD1306_ACLL(transmit, data, 100);
  AXK_SSD1306_ACLL(cs_set);
  AXK_SSD1306_ACLL(dc_set);
#endif
}
