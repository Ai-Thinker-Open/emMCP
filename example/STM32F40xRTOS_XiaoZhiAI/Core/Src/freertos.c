/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "cJSON.h"
#include "emMCP.h"
#include "log.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "ws2812.h"
#include <stdbool.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
Cozmo cozmo;
char long_text[128] = "坐等你使用\"你好小安\"唤醒我!";

osThreadId_t u8g2Demo_TaskHandle;

const osThreadAttr_t u8g2Demo_Task_attributes = {
    .name = "u8g2Demo",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

emMCP_tool_t ws2812_tool;
extern ws2812_strip_t ws2812_strip;
static bool enbaled = false;
void U8g2DemoTask(void *argument);


/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  u8g2Demo_TaskHandle = osThreadNew(U8g2DemoTask, NULL, &u8g2Demo_Task_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * 
 * @param arg 
 */
static void ws2812_tool_set_request_handler(void *arg)
{
  cJSON *root = (cJSON *)arg;
  
  // 开关功能
  cJSON *strip_switch = cJSON_GetObjectItemCaseSensitive(root, "strip_switch");
  if (strip_switch != NULL) {
    enbaled = strip_switch->valueint;//缓存状态
  }
  // 亮度功能
  cJSON *brightness = cJSON_GetObjectItemCaseSensitive(root, "brightness");
  if (brightness != NULL) {
    ws2812_strip.brightness = brightness->valuedouble;//更新亮度
  }
  // 灯珠和颜色功能
  cJSON *strip_id = cJSON_GetObjectItemCaseSensitive(root, "strip_count");
  cJSON *red = cJSON_GetObjectItemCaseSensitive(root, "red");
  cJSON *green = cJSON_GetObjectItemCaseSensitive(root, "green");
  cJSON *blue = cJSON_GetObjectItemCaseSensitive(root, "blue");

  if (strip_id != NULL && red != NULL && green != NULL && blue != NULL) {
    // 缓存颜色值
    ws2812_strip.dev->color.r = red->valueint;
    ws2812_strip.dev->color.g = green->valueint;
    ws2812_strip.dev->color.b = blue->valueint;
    if (strip_id->valueint==-1) {
      ws2812_set_all_pixels_color(ws2812_strip.dev->color.r,
      ws2812_strip.dev->color.g, ws2812_strip.dev->color.b,
      enbaled ? ws2812_strip.brightness : 0.0f);
    }else {
      ws2812_set_pixel_color(strip_id->valueint, ws2812_strip.dev->color.r,
      ws2812_strip.dev->color.g, ws2812_strip.dev->color.b);
    }
  }
  // 刷新灯珠
  ws2812_show_leds();
  // 返回成功
  emMCP_ResponseValue(emMCP_CTRL_OK);
}
/**
 * @brief Function implementing the defaultTask thread.
 * 
 * @param arg 
 */
static void ws2812_tool_check_request_handler(void *arg)
{
  
}
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  ws2812_tool.name = "ws2812灯条";
  ws2812_tool.description = "用于控制WS2812RGB灯条的工具";
  ws2812_tool.setRequestHandler = ws2812_tool_set_request_handler;
  ws2812_tool.checkRequestHandler = ws2812_tool_check_request_handler;
  ws2812_tool.inputSchema.properties[0].name = "strip_count";
  ws2812_tool.inputSchema.properties[0].description = "灯珠的id:0~2,控制所有灯珠时发送-1";
  ws2812_tool.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_tool.inputSchema.properties[1].name = "red";
  ws2812_tool.inputSchema.properties[1].description = "红色值(0-255)";
  ws2812_tool.inputSchema.properties[1].type = MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_tool.inputSchema.properties[2].name = "green";
  ws2812_tool.inputSchema.properties[2].description = "绿色值(0-255)";
  ws2812_tool.inputSchema.properties[2].type = MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_tool.inputSchema.properties[3].name = "blue";
  ws2812_tool.inputSchema.properties[3].description = "蓝色值(0-255)";
  ws2812_tool.inputSchema.properties[3].type = MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_tool.inputSchema.properties[4].name = "brightness";
  ws2812_tool.inputSchema.properties[4].description = "亮度 0.00~1.0,关闭设置为0.00,查询时为null";
  ws2812_tool.inputSchema.properties[4].type = MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_tool.inputSchema.properties[5].name = "strip_switch";
  ws2812_tool.inputSchema.properties[5].description = "灯条开关,false:关闭 true:打开,查询时为null";
  ws2812_tool.inputSchema.properties[5].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
  emMCP_AddToolToToolList(&ws2812_tool);
  emMCP_RegistrationTools();


  for (;;) {
    emMCP_TickHandle(10);
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */



void U8g2DemoTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
    int16_t scroll_offset = 0;        // 初始偏移量（屏幕右侧外）
    const uint8_t scroll_speed = 1;   // 滚动速度（像素/帧）
    const uint16_t frame_delay = 10;  // 帧间隔（毫秒）

    uint16_t screen_width = u8g2_GetDisplayWidth(&u8g2);

    //开机图标，显示一个小机器人，encoding=0x32，其他值参考：https://github.com/olikraus/u8g2/wiki/fntgrpstreamline
    u8g2_SetFont(&u8g2, u8g2_font_streamline_technology_t);
    u8g2_DrawGlyphX2(&u8g2, 40, 54, 0x32);
    u8g2_SendBuffer(&u8g2);
    HAL_Delay(1000);
    scroll_offset = screen_width;
    for (;;) {
     u8g2_ClearBuffer(&u8g2);  // 清空缓冲区

     // 1. 重绘静态文本（必须在循环内重新绘制，否则会被清除）
     u8g2_SetFont(&u8g2, current_emotion==U8G2_EMOTION_MUSIC?u8g2_font_streamline_music_audio_t:u8g2_font_emoticons21_tr);
     
    u8g2_DrawGlyphX2(&u8g2, 42, 44, current_emotion==U8G2_EMOTION_MUSIC?(U8G2_EMOTION_MUSIC&0xFF):current_emotion);
     
     // 2. 绘制滚动文本
    u8g2_SetFont(&u8g2,u8g2_font_wqy14_t_gb2312);
    scroll_text(&u8g2, long_text, 62, scroll_offset);
    scroll_offset += scroll_speed;
    u8g2_SendBuffer(&u8g2);
    osDelay(frame_delay);
  }
  /* USER CODE END StartDefaultTask */
}
/* USER CODE END Application */

