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
#include "cmsis_os.h"
#include "main.h"
#include "task.h"
#include "ws2812.h"

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
    .priority = (osPriority_t)osPriorityNormal,
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

emMCP_tool_t ws2812_switch_tool;
emMCP_tool_t ws2812_color_tool;
emMCP_tool_t ws2812_brightness_tool;
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
  defaultTaskHandle =
      osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  u8g2Demo_TaskHandle =
      osThreadNew(U8g2DemoTask, NULL, &u8g2Demo_Task_attributes);
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
static void ws2812_tool_set_request_handler(void *arg) {
  cJSON *root = (cJSON *)arg;
  cJSON *ws2812_enabled_item = cJSON_GetObjectItem(root, "ws2812_enabled");
  if (ws2812_enabled_item != NULL) {
    ws2812_set_all_pixels_color(
        ws2812_strip.dev->color.r, ws2812_strip.dev->color.g,
        ws2812_strip.dev->color.b,
        ws2812_enabled_item->valueint ? ws2812_strip.brightness : 0.0f);
    enbaled = ws2812_enabled_item->valueint;
  }
  emMCP_ResponseValue(emMCP_CTRL_OK);
}
/**
 * @brief Function implementing the defaultTask thread.
 *
 * @param arg
 */
static void ws2812_tool_check_request_handler(void *arg) {
  char *response_str = pvPortMalloc(32);
  memset(response_str, 0, 32);
  sprintf(response_str, "{\"enabled\":%s}", enbaled ? "true" : "false");
  emMCP_ResponseValue(response_str);
}

static void ws2812_tool_set_color_request_handler(void *arg) {
  cJSON *root = (cJSON *)arg;
  cJSON *red_item = cJSON_GetObjectItem(root, "red");
  cJSON *green_item = cJSON_GetObjectItem(root, "green");
  cJSON *blue_item = cJSON_GetObjectItem(root, "blue");

  if (red_item != NULL && green_item != NULL && blue_item != NULL) {
    ws2812_strip.dev->color.r = red_item->valueint;
    ws2812_strip.dev->color.g = green_item->valueint;
    ws2812_strip.dev->color.b = blue_item->valueint;
  }

  cJSON *led_id = cJSON_GetObjectItem(root, "led_id");
  if (led_id != NULL) {
    if (led_id->valueint == -1) {
      ws2812_set_all_pixels_color(
          ws2812_strip.dev->color.r, ws2812_strip.dev->color.g,
          ws2812_strip.dev->color.b, enbaled ? ws2812_strip.brightness : 0.0f);
    } else {
      ws2812_set_pixel_color(led_id->valueint, ws2812_strip.dev->color.r,
                             ws2812_strip.dev->color.g,
                             ws2812_strip.dev->color.b);
      ws2812_show_leds();
    }
  } else {
    enbaled = true;
    ws2812_set_all_pixels_color(
        ws2812_strip.dev->color.r, ws2812_strip.dev->color.g,
        ws2812_strip.dev->color.b, enbaled ? ws2812_strip.brightness : 0.0f);
    ws2812_show_leds();
  }
  emMCP_ResponseValue(emMCP_CTRL_OK);
}

static void ws2812_tool_check_color_request_handler(void *arg) {

  emMCP_ResponseValue(emMCP_CTRL_OK);
}

static void ws2812_tool_set_brightness_request_handler(void *arg) {
  cJSON *root = (cJSON *)arg;
  cJSON *brightness_item = cJSON_GetObjectItem(root, "set_brightness");
  if (brightness_item != NULL) {
    enbaled = true;
    float brightness = brightness_item->valuedouble / 100.0f;
    if (brightness < 0.0f)
      brightness = 0.0f;
    if (brightness > 1.0f)
      brightness = 1.0f;
    ws2812_strip.brightness = brightness;
    ws2812_set_all_pixels_color(
        ws2812_strip.dev->color.r, ws2812_strip.dev->color.g,
        ws2812_strip.dev->color.b, enbaled ? ws2812_strip.brightness : 0.0f);
  } else {
    return;
  }
  emMCP_ResponseValue(emMCP_CTRL_OK);
}

static void ws2812_tool_check_brightness_request_handler(void *arg) {
  char *response_str = pvPortMalloc(32);
  memset(response_str, 0, 32);
  sprintf(response_str, "%d", (int)(ws2812_strip.brightness * 100));
  emMCP_ResponseValue(response_str);
}
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  ws2812_switch_tool.name = "ws2812.switch";
  ws2812_switch_tool.description = "灯条的开关控制";
  ws2812_switch_tool.inputSchema.properties[0].name = "ws2812_enabled";
  ws2812_switch_tool.inputSchema.properties[0].description =
      "打开为true,关闭为false,查询为null";
  ws2812_switch_tool.inputSchema.properties[0].type =
      MCP_SERVER_TOOL_TYPE_BOOLEAN;
  ws2812_switch_tool.setRequestHandler = ws2812_tool_set_request_handler;
  ws2812_switch_tool.checkRequestHandler = ws2812_tool_check_request_handler;

  ws2812_color_tool.name = "ws2812.color";
  ws2812_color_tool.description = "灯条的颜色控制";
  ws2812_color_tool.inputSchema.properties[0].name = "red";
  ws2812_color_tool.inputSchema.properties[0].description = "红色值0-255";
  ws2812_color_tool.inputSchema.properties[0].type =
      MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_color_tool.inputSchema.properties[1].name = "green";
  ws2812_color_tool.inputSchema.properties[1].description = "绿色值0-255";
  ws2812_color_tool.inputSchema.properties[1].type =
      MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_color_tool.inputSchema.properties[2].name = "blue";
  ws2812_color_tool.inputSchema.properties[2].description = "蓝色值0-255";
  ws2812_color_tool.inputSchema.properties[2].type =
      MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_color_tool.inputSchema.properties[3].name = "led_id";
  ws2812_color_tool.inputSchema.properties[3].description =
      "灯珠索引0~3,不传表示全部";
  ws2812_color_tool.inputSchema.properties[3].type =
      MCP_SERVER_TOOL_TYPE_NUMBER;

  ws2812_color_tool.setRequestHandler = ws2812_tool_set_color_request_handler;
  ws2812_color_tool.checkRequestHandler =
      ws2812_tool_check_color_request_handler;

  ws2812_brightness_tool.name = "ws2812.brightness";
  ws2812_brightness_tool.description = "灯条的亮度控制";
  ws2812_brightness_tool.inputSchema.properties[0].name = "set_brightness";
  ws2812_brightness_tool.inputSchema.properties[0].description = "亮度值0-100";
  ws2812_brightness_tool.inputSchema.properties[0].type =
      MCP_SERVER_TOOL_TYPE_NUMBER;
  ws2812_brightness_tool.inputSchema.properties[1].name = "get_brightness";
  ws2812_brightness_tool.inputSchema.properties[1].description = "查询亮度";
  ws2812_brightness_tool.inputSchema.properties[1].type =
      MCP_SERVER_TOOL_TYPE_NULL;
  ws2812_brightness_tool.setRequestHandler =
      ws2812_tool_set_brightness_request_handler;
  ws2812_brightness_tool.checkRequestHandler =
      ws2812_tool_check_brightness_request_handler;
  emMCP_AddToolToToolList(&ws2812_switch_tool);
  emMCP_AddToolToToolList(&ws2812_color_tool);
  emMCP_AddToolToToolList(&ws2812_brightness_tool);
  emMCP_RegistrationTools();

  for (;;) {
    emMCP_TickHandle(10);
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void U8g2DemoTask(void *argument) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  int16_t scroll_offset = 0;       // 初始偏移量（屏幕右侧外）
  const uint8_t scroll_speed = 1;  // 滚动速度（像素/帧）
  const uint16_t frame_delay = 10; // 帧间隔（毫秒）

  uint16_t screen_width = u8g2_GetDisplayWidth(&u8g2);

  // 开机图标，显示一个小机器人，encoding=0x32，其他值参考：https://github.com/olikraus/u8g2/wiki/fntgrpstreamline
  u8g2_SetFont(&u8g2, u8g2_font_streamline_technology_t);
  u8g2_DrawGlyphX2(&u8g2, 40, 54, 0x32);
  u8g2_SendBuffer(&u8g2);
  HAL_Delay(1000);
  scroll_offset = screen_width;
  for (;;) {
    u8g2_ClearBuffer(&u8g2); // 清空缓冲区

    // 1. 重绘静态文本（必须在循环内重新绘制，否则会被清除）
    u8g2_SetFont(&u8g2, current_emotion == U8G2_EMOTION_MUSIC
                            ? u8g2_font_streamline_music_audio_t
                            : u8g2_font_emoticons21_tr);

    u8g2_DrawGlyphX2(&u8g2, 42, 44,
                     current_emotion == U8G2_EMOTION_MUSIC
                         ? (U8G2_EMOTION_MUSIC & 0xFF)
                         : current_emotion);

    // 2. 绘制滚动文本
    u8g2_SetFont(&u8g2, u8g2_font_wqy14_t_gb2312);
    scroll_text(&u8g2, long_text, 62, scroll_offset);
    scroll_offset += scroll_speed;
    u8g2_SendBuffer(&u8g2);
    osDelay(frame_delay);
  }
  /* USER CODE END StartDefaultTask */
}
/* USER CODE END Application */
