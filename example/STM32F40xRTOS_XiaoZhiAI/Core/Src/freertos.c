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
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

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
    .stack_size = 256 * 4,
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

// 位反转函数
void GT20_To_U8g2Buffer(uint8_t *gt20_data, uint8_t *u8g2_buffer) {
    // 将竖置横排转换为U8g2需要的格式
    for(uint8_t i=0; i<16; i++) {  // 16行
        u8g2_buffer[i] = gt20_data[i];
    }
}
void U8g2DemoTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
    int16_t scroll_offset = 0;        // 初始偏移量（屏幕右侧外）
    const uint8_t scroll_speed = 1;   // 滚动速度（像素/帧）
    const uint16_t frame_delay = 10;  // 帧间隔（毫秒）
    // uint16_t screen_width = u8g2_GetDisplayWidth(&u8g2);
    // uint8_t u8g2_buffer[32] = {0};
    // // u8g2_DrawUTF8FromGT20l16(&u8g2,0,32,"乐");
    // // GT20_To_U8g2Buffer(le_col_data, u8g2_buffer);
    // u8g2_ClearBuffer(&u8g2);
    // // u8g2_DrawXBMP(&u8g2, 10, 0, 8, 1,  le_font_fixed);  // 宽8，高1
    // // u8g2_DrawXBMP(&u8g2, 0, 0, 16, 16, bitmap_bytes);
    // // u8g2_DrawBitmap(&u8g2, 0, 0, 1, 16, u8g2_buffer);
    // // u8g2_SendBuffer(&u8g2);
    // scroll_offset = screen_width;
    // u8g2_SetFont(&u8g2, u8g2_font_wqy16_t_gb2312); 
    for (;;) {
      // u8g2_ClearBuffer(&u8g2);
     
      // scroll_text(&u8g2, long_text, 62, scroll_offset);
      // scroll_offset += scroll_speed;
      // // // u8g2_DrawUTF8FromGT20l16(&u8g2,0,32,"乐");
      // u8g2_SendBuffer(&u8g2); 
      osDelay(frame_delay);
  }
  /* USER CODE END StartDefaultTask */
}
/* USER CODE END Application */

