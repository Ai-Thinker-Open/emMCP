/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
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
#include "log.h"
#include "main.h"
#include "stm32f10x_bsp_i2c.h"
#include "task.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "axk_ch224.h"
#include "axk_sht3x.h"
#include "axk_ssd1306.h"
#include "axk_ws2812.h"
#include "emMCP.h"
#include "relay.h"
#include "uartPort.h"
#include "usart.h"
#include <stdint.h>
#include <time.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFFSER_MAX_SIZE 256
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
/* Definitions for sht3x_read */
osThreadId_t sht3x_readHandle;
const osThreadAttr_t sht3x_read_attributes = {
    .name = "sht3x_read",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal1,
};
/* Definitions for ws2812_mode */
osThreadId_t ws2812_modeHandle;
const osThreadAttr_t ws2812_mode_attributes = {
    .name = "ws2812_mode",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal2,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
static uint8_t rxBuffer[RXBUFFSER_MAX_SIZE] = {0};
static emMCP_t emMCP;
axk_ws2812_strip_t ws2812 = {.led_count = WS2812_MAX_NUM};
color_t RED = {0xff, 0x00, 0x00};
color_t GREEN = {0x00, 0xff, 0x00};
color_t BLUE = {0x00, 0x00, 0xff};
bool sht30_is_init = false;
bool ch224_is_init = false;
double temperature = 0.0;
double humidity = 0.0;
static void smoothcolorTransition_callbark(color_t color, void *arg);
static void relay_tool_callbark(void* arg);
static void sht3x_tool_callbark(void* arg);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void sht3x_read_task(void *argument);
void ws2812_modeTask(void *argument);

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

  /* creation of sht3x_read */
  sht3x_readHandle = osThreadNew(sht3x_read_task, NULL, &sht3x_read_attributes);

  /* creation of ws2812_mode */
  ws2812_modeHandle =
      osThreadNew(ws2812_modeTask, NULL, &ws2812_mode_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
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
void StartDefaultTask(void *argument) {
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *)rxBuffer, sizeof(rxBuffer));
  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
  emMCP_Init(&emMCP);

  static emMCP_tool_t relay_tool={
    .name = "self.relay",
    .description="Relay status control and query tool",
    .inputSchema.properties[0].name = "relay.enable",
    .inputSchema.properties[0].description = "Relay switch, true is on, false is off",
    .inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN,
    .inputSchema.properties[1].name= "relay.status",
    .inputSchema.properties[1].description = "Relay status, check relay status,null is check",
    .inputSchema.properties[1].type = MCP_SERVER_TOOL_TYPE_BOOLEAN,
    .checkRequestHandler=relay_tool_callbark,
    .setRequestHandler=relay_tool_callbark,
  };
  static emMCP_tool_t sht3x_tool = {
    .name = "self.sht3x",
    .description = "SHT3X temperature and humidity sensor tool",
    .inputSchema.properties[0].name = "sht3x.temprature",
    .inputSchema.properties[0].description =
        "Read SHT3X temperature,null is read,accuracy of 0.1",
    .inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_NUMBER,
    .inputSchema.properties[1].name = "sht3x.humidity",
    .inputSchema.properties[1].description =
        "Read SHT3X humidity,null is read",
    .inputSchema.properties[1].type = MCP_SERVER_TOOL_TYPE_NUMBER,
    .checkRequestHandler=sht3x_tool_callbark,
    .setRequestHandler=sht3x_tool_callbark,
  };
  emMCP_AddToolToToolList(&relay_tool);
  emMCP_AddToolToToolList(&sht3x_tool);
  
  emMCP_RegistrationTools();

  for (;;) {
    // osDelay();
    emMCP_TickHandle(10);
  }
  /* USER CODE END StartDefaultTask */
}
/* USER CODE BEGIN Header_sht3x_read_task */
/**
 * @brief Function implementing the sht3x_read thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_sht3x_read_task */
void sht3x_read_task(void *argument) {
  /* USER CODE BEGIN sht3x_read_task */
  /* Infinite loop */

  uint8_t res = axk_sht3x_init();
  if (res != 0) {
    log_error("sht3x not driver");
    sht30_is_init = false;
  } else {
    sht30_is_init = true;
    log_info("sht3x init OK!");
  }

  res = axk_ch224_init();
  if (res != 0) {
    log_error("ch224 not driver");
    ch224_is_init = false;
  } else {
    ch224_is_init = true;
    log_info("ch224 init OK!");
      int ch224_status = axk_ch224_get_status(AXK_CH224_REG_STATUS);

    if (ch224_status < 0) {
      log_error("ch224 status error: %d", ch224_status);
    } else
      log_info("ch224 status: 0x%02X", ch224_status);

    // // 设置CH224输出电压为5V
    // ch224_status = axk_ch224_set_vout(AXK_CH224_VOUT_AVS);
    ch224_status = axk_ch224_set_pps_vout(12.2);
    if (ch224_status < 0) {
      log_error("ch224 set avs vout error: %d", ch224_status);
    } else
      log_info("ch224 set avs vout OK!");
    ch224_status = axk_ch224_set_mode(AXK_CH224_VOUT_PPS);
    if (ch224_status < 0) {
      log_error("ch224 set mode error: %d", ch224_status);
    } else {
      log_info("ch224 set mode OK!");
    }
  }
  axk_relay_init();
 
  // 初始化WS2812灯条

 
  for (;;) {

    osDelay(pdMS_TO_TICKS(1000));
    // axk_relay_toggle();
    if (!sht30_is_init) {
      continue;
    }
    res = axk_sht3x_read(0x2c06, &temperature, &humidity);
    if (res != 0) {
      log_error("sht3x read error: %d", res);
      continue;
    }
    log_info("sht30: %0.2d C,%0.2d %%", (uint8_t)temperature,
             (uint8_t)humidity);
  }
  /* USER CODE END sht3x_read_task */
}
/* USER CODE BEGIN Header_ws2812_modeTask */
/**
 * @brief Function implementing the ws2812_mode thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_ws2812_modeTask */
void ws2812_modeTask(void *argument) {
  /* USER CODE BEGIN ws2812_modeTask */
  /* Infinite loop */
  delay_ms(50);
  axk_ssd1306_init();
  axk_ssd1306_set_color_turn(0);
  axk_ssd1306_set_display_turn(0);
  axk_ssd1306_clear_screen();
  axk_ssd1306_clear_screen();
  axk_ssd1306_show_utf8_str(32, 0, "欢迎使用");
  axk_ssd1306_show_utf8_str(24, 3, "九章开发板");
  axk_ws2812_init(&ws2812);
  for (;;) {
    smoothcolorTransition(RED, BLUE, 500, smoothcolorTransition_callbark, NULL);
    smoothcolorTransition(BLUE, RED, 500, smoothcolorTransition_callbark, NULL);
    // osDelay(1);
  }
  /* USER CODE END ws2812_modeTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart == &huart2) {
    // 确保接收到的数据以 null 结尾，防止 strlen 越界
    if (Size < RXBUFFSER_MAX_SIZE) {
      rxBuffer[Size] = '\0';
    } else {
      rxBuffer[RXBUFFSER_MAX_SIZE - 1] = '\0';
    }
    
    uartPortRecvData((char *)rxBuffer, Size);

    // 重新启动 DMA 接收
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, RXBUFFSER_MAX_SIZE);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
  }
}

static void smoothcolorTransition_callbark(color_t color, void *arg) {
  axk_ws2812_set_all_pixels_color(color.r, color.g, color.b,
                                  axk_ws2812_strip_dev->brightness);
  vTaskDelay(pdMS_TO_TICKS(5));
}


static void relay_tool_callbark(void* arg) 
{

  if(arg==NULL) {
    log_error("relay_tool_callbark arg is null");
    return;
  }
  cJSON* param=(cJSON*)arg;
  cJSON* relay_enable=cJSON_GetObjectItemCaseSensitive(param,"relay.enable");
  cJSON* relay_status=cJSON_GetObjectItemCaseSensitive(param,"relay.status");

  if(relay_enable!=NULL){
    log_info("relay_enable: %s", relay_enable->valueint? "true":"false");
    axk_relay_set(relay_enable->valueint);
    emMCP_ResponseValue(relay_enable->valueint?"{\"relay.enable\":true}":"{\"relay.enable\":false}");
  }

  if(relay_status!=NULL){
    emMCP_ResponseValue( axk_relay_get()?"{\"relay.status\":true}":"{\"relay.status\":false}");
  }
}

static void sht3x_tool_callbark(void* arg) {
  if(arg==NULL){
    log_error("sht3x_tool_callbark arg is null");
    return;
  }
  cJSON* param=(cJSON*)arg;
  log_debug("sht3x_tool_callbark ");
  cJSON* temprature_item=cJSON_GetObjectItemCaseSensitive(param,"sht3x.temprature");
  cJSON* humidity_item=cJSON_GetObjectItemCaseSensitive(param,"sht3x.humidity");
  if(temprature_item!=NULL||humidity_item!=NULL){
    if(sht30_is_init){
      char response[64];
      snprintf(response, sizeof(response),
               "{\"temperature\":%.1f,\"humidity\":%.1f}",temperature, humidity);
      log_info("sht3x_tool_callbark response: %s", response);
      emMCP_ResponseValue(response);
    }
    
  }
  else {
    log_error("sht3x_tool_callbark read is null");
  }
}

/* USER CODE END Application */