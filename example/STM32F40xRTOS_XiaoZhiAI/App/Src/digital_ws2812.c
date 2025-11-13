/**
 * @file digital_WS2812.c
 * @author SeaHi-Mo (Seahi-Mo@Foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-11-12
 *
 * @copyright Copyright (c) 2025
 *
 */
#include "digital_ws2812.h"
#include "log.h"
#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include <stddef.h>
#include <stdint.h>

uint8_t ws2812_buffer[WS2812_LED_NUM*24]; // 支持最多24个WS2812 LED

void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

// 纳秒级延时
void delay_ns(uint32_t ns)
{
    uint32_t start_tick = DWT->CYCCNT;
    uint32_t delay_ticks = (ns * (SystemCoreClock / 1000000)) / 1000;
    
    while((DWT->CYCCNT - start_tick) < delay_ticks);
}

static void ws2812_write_bit(uint8_t bit)
{

     for(int i = 0; i < 8; i++)
    {
        if((bit<<1) & 0x80) // 当前位为1
        {
            HAL_GPIO_WritePin(WS2812_Pin_GPIO_Port, WS2812_Pin_Pin, GPIO_PIN_SET);
            delay_ns(400); // T1H = 0.833us
            HAL_GPIO_WritePin(WS2812_Pin_GPIO_Port, WS2812_Pin_Pin, GPIO_PIN_RESET);
            delay_ns(60); // T1L = 0.417us
        }
        else // 当前位为0
        {
            HAL_GPIO_WritePin(WS2812_Pin_GPIO_Port, WS2812_Pin_Pin, GPIO_PIN_SET);
            delay_ns(60); // T0H = 0.417us
            HAL_GPIO_WritePin(WS2812_Pin_GPIO_Port, WS2812_Pin_Pin, GPIO_PIN_RESET);
            delay_ns(400); // T0L = 0.833us
        }
    }
    
}

/**
 * @brief 初始化WS2812 LED灯带
 * 
 * @param ws2812_strip 
 */
void ws281x_digital_init(ws2812_strip_t *ws2812_strip)
{
   if(ws2812_strip == NULL || ws2812_strip->led_count == 0)
   {
       return;
   }
   if (ws2812_strip->dev == NULL) {
    ws2812_strip->dev = pvPortMalloc(sizeof(ws2812_dev_t) * ws2812_strip->led_count);
    ws2812_strip->brightness = 0.5;
    for (uint8_t i = 0; i < ws2812_strip->led_count; i++)
    {
        ws2812_strip->dev[i].index = i;
        ws2812_strip->dev[i].brightness = &ws2812_strip->brightness;
        ws2812_strip->dev[i].color.r = 255;
        ws2812_strip->dev[i].color.g = 255;
        ws2812_strip->dev[i].color.b = 255;
    }
   }
    DWT_Init();
   HAL_GPIO_WritePin(WS2812_Pin_GPIO_Port, WS2812_Pin_Pin, GPIO_PIN_RESET);
}
/**
 * @brief 更新WS2812 LED灯带
 * 
 */
void ws281x_digital_show_leds(void) {
  for (size_t i=0; i< WS2812_LED_NUM*3; i++) {
    ws2812_write_bit(ws2812_buffer[i]);
  }
   HAL_GPIO_WritePin(WS2812_Pin_GPIO_Port,WS2812_Pin_Pin, GPIO_PIN_RESET);
}
/**
 * @brief 设置单个LED的颜色
 * 
 * @param led_index 
 * @param red 
 * @param green 
 * @param blue 
 */
void ws281x_digital_set_pixel_color(uint8_t led_index, uint8_t red, uint8_t green, uint8_t blue) 
{
    if(led_index >= WS2812_LED_NUM) {
        return; // 超出范围
    }
    // WS2812的颜色顺序是GRB
    ws2812_buffer[led_index * 3] = green;
    ws2812_buffer[led_index * 3 + 1] = red;
    ws2812_buffer[led_index * 3 + 2] = blue;
}


