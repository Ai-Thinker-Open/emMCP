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
#include "stm32f4xx_hal.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include "u8g2.h"

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
char long_text[128] = "睡眠中,请使用\"你好小安\"唤醒我!";
osThreadId_t u8g2Demo_TaskHandle;
const osThreadAttr_t u8g2Demo_Task_attributes = {
    .name = "u8g2Demo",
    .stack_size = 512 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

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
   u8g2Demo_TaskHandle= osThreadNew(U8g2DemoTask, NULL, &u8g2Demo_Task_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
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
  for (;;) {
    emMCP_TickHandle(10);
    osDelay(10);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
// e0 f0 98 8c 98 f0 e0 00 0f 0f 00 00 00 0f 0f 00


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
    u8g2_SetFont(&u8g2,u8g2_font_emoticons21_tr);
    u8g2_DrawGlyphX2(&u8g2, 42, 44, current_emotion);
    u8g2_SetFont(&u8g2,u8g2_font_wqy14_t_gb2312);
    scroll_text(&u8g2, long_text, 62, scroll_offset);
    scroll_offset += scroll_speed;
    u8g2_SendBuffer(&u8g2);
    osDelay(frame_delay);
  }
  /* USER CODE END StartDefaultTask */
}
/* USER CODE END Application */

