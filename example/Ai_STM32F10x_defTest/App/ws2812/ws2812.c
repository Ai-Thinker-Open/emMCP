
#include "ws2812.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"
#include <math.h>
#include <stdint.h>

// 全局变量定义
ws2812_color_t ws2812_leds[WS2812_LED_NUM];
static uint16_t ws2812_dma_buffer[WS2812_BUFFER_SIZE];

/**
 * @brief 初始化WS2812控制
 */
void ws2812_init(void) {
  // 初始化所有LED为关闭状态
  for (int i = 0; i < WS2812_LED_NUM; i++) {
    ws2812_leds[i].r = 0;
    ws2812_leds[i].g = 0;
    ws2812_leds[i].b = 0;
  }
}

/**
 * @brief 将RGB值转换为WS2812时序数据
 */
static void convert_rgb_to_ws2812_data(uint8_t r, uint8_t g, uint8_t b,
                                       uint16_t *buffer) {
  // WS2812数据格式是GRB（绿色、红色、蓝色）
  uint32_t data = ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;

  // 每个LED需要24位数据（从最高位开始）
  for (int i = 23; i >= 0; i--) {
    if (data & (1 << i)) {
      buffer[(23 - i) * 2] = CODE1;     // 高电平持续时间长
      buffer[(23 - i) * 2 + 1] = CODE0; // 低电平持续时间短
    } else {
      buffer[(23 - i) * 2] = CODE0;     // 高电平持续时间短
      buffer[(23 - i) * 2 + 1] = CODE1; // 低电平持续时间长
    }
  }
}

/**
 * @brief 设置单个像素的颜色
 */
void ws2812_set_pixel(uint16_t index, uint8_t r, uint8_t g, uint8_t b) {
  if (index < WS2812_LED_NUM) {
    ws2812_leds[index].r = r;
    ws2812_leds[index].g = g;
    ws2812_leds[index].b = b;
  }
}

/**
 * @brief 填充所有LED为同一颜色
 */
void ws2812_fill(uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < WS2812_LED_NUM; i++) {
    ws2812_leds[i].r = r;
    ws2812_leds[i].g = g;
    ws2812_leds[i].b = b;
  }
}

/**
 * @brief 更新指定数量的WS2812 LED
 */
void ws2812_update_with_num(uint16_t led_num) {
  if (led_num > WS2812_LED_NUM) {
    led_num = WS2812_LED_NUM;
  }

  uint16_t *buffer_ptr = ws2812_dma_buffer;
  uint16_t total_bits = 0;

  // 为每个LED生成对应的时序数据
  for (int i = 0; i < led_num; i++) {
    convert_rgb_to_ws2812_data(ws2812_leds[i].r, ws2812_leds[i].g,
                               ws2812_leds[i].b, buffer_ptr);
    buffer_ptr += 24 * 2; // 每个LED需要24*2个时隙
    total_bits += 24 * 2;
  }

  // 添加RESET信号（至少50us低电平）
  ws2812_dma_buffer[total_bits] = CODE_RESET;
  total_bits++;

  // 启动DMA传输
  HAL_TIM_PWM_Start_DMA(&htim1, TIM_CHANNEL_4, (uint32_t *)ws2812_dma_buffer,
                        total_bits);
}

// 彩虹效果测试函数

void ws2812_update(void) { ws2812_update_with_num(WS2812_LED_NUM); }

void ws2812_rainbow_test(void) {
  // 创建彩虹效果
  for (int i = 0; i < WS2812_LED_NUM; i++) {
    float hue = (i * 360.0 / WS2812_LED_NUM); // 色相分布

    // 简单的HSV到RGB转换
    float c = 50.0; // Chroma
    float x = c * (1 - fabs(fmod(hue / 60.0, 2) - 1));
    float m = 0.0; // Lightness adjustment

    if (hue >= 0 && hue < 60) {
      ws2812_leds[i].r = (uint8_t)(c + m);
      ws2812_leds[i].g = (uint8_t)(x + m);
      ws2812_leds[i].b = (uint8_t)m;
    } else if (hue >= 60 && hue < 120) {
      ws2812_leds[i].r = (uint8_t)(x + m);
      ws2812_leds[i].g = (uint8_t)(c + m);
      ws2812_leds[i].b = (uint8_t)m;
    } else if (hue >= 120 && hue < 180) {
      ws2812_leds[i].r = (uint8_t)m;
      ws2812_leds[i].g = (uint8_t)(c + m);
      ws2812_leds[i].b = (uint8_t)(x + m);
    } else if (hue >= 180 && hue < 240) {
      ws2812_leds[i].r = (uint8_t)m;
      ws2812_leds[i].g = (uint8_t)(x + m);
      ws2812_leds[i].b = (uint8_t)(c + m);
    } else if (hue >= 240 && hue < 300) {
      ws2812_leds[i].r = (uint8_t)(x + m);
      ws2812_leds[i].g = (uint8_t)m;
      ws2812_leds[i].b = (uint8_t)(c + m);
    } else {
      ws2812_leds[i].r = (uint8_t)(c + m);
      ws2812_leds[i].g = (uint8_t)m;
      ws2812_leds[i].b = (uint8_t)(x + m);
    }
  }

  ws2812_update();
}