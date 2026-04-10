/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "cJSON.h"
#include "dma.h"
#include "gpio.h"
#include "stm32f1xx_hal_uart.h"
#include "uartPort.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "emMCP.h"
#include <stdint.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UART_RXBUFFER_SIZE 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t RxBuffer[UART_RXBUFFER_SIZE] = {0};
emMCP_t emMCP;
emMCP_tool_t rgb; // 创建工具
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart == &huart2) {

    uartPortRecvData((char *)RxBuffer, Size);
    HAL_UARTEx_ReceiveToIdle_DMA(huart, RxBuffer, UART_RXBUFFER_SIZE);
    __HAL_DMA_DISABLE_IT(huart->hdmarx, DMA_IT_HT);
  }
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void emMCP_SetRGBHandler(void *arg) {
    cJSON *param = (cJSON *)arg;
    cJSON *enable = cJSON_GetObjectItemCaseSensitive(param, "enable");
    cJSON *ch_red = cJSON_GetObjectItemCaseSensitive(param, "red");
    cJSON *ch_green = cJSON_GetObjectItemCaseSensitive(param, "green");
    cJSON *ch_blue = cJSON_GetObjectItemCaseSensitive(param, "blue");
    if (enable != NULL) {
        if (cJSON_IsTrue(enable)) {
            log_info("RGB enable is true");
        } else {
            log_info("RGB enable is false");
        }
    }
    if (ch_red != NULL) {
        log_info("RGB red is %d", ch_red->valueint);
    }
    if (ch_green != NULL) {
        log_info("RGB green is %d", ch_green->valueint);
    }
    if (ch_blue != NULL) {
        log_info("RGB blue is %d", ch_blue->valueint);
    }
    // 返回控制结果
    char *result = malloc(128);
    sprintf(result, " {\"%s\":%s,\"%s\":%d,\"%s\":%d,\"%s\":%d}",
            enable->string,
            enable->valueint ? "true" : "false",
            ch_red->string, ch_red->valueint,
            ch_green->string, ch_green->valueint,
            ch_blue->string, ch_blue->valueint);
    emMCP_ResponseValue(result);
    free(result);
}
void emMCP_GetRGBHandler(void *arg) {}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU
   * Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the
   * Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, RxBuffer, UART_RXBUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);

  emMCP_Init(&emMCP);
  // emMCP_SetAiVolume(50);
  emMCP_CheckAiVolume();
  rgb.name = "RGB彩灯";                      // 工具名称，保持唯一性
  rgb.description = "用来控制RGB彩灯的亮灭"; // 工具的功能描述
  rgb.inputSchema.properties[0].name = "enable"; // 属性指令，AI 通过这个指令发送命令
  rgb.inputSchema.properties[0].description = "是否打开RGB彩灯,true表示打开，false表示关闭查询时为null"; // 指令描述，AI 通过这个描述理解指令
  rgb.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN; // 指令类型，AI通过这个类型发送相对应的数据
  rgb.inputSchema.properties[1].name = "red"; // 属性指令，AI 通过这个指令发送命令
  rgb.inputSchema.properties[1].description = "RGB彩灯的红色,范围0-255,查询时为null"; // 指令描述，AI通过这个描述理解指令
  rgb.inputSchema.properties[1].type = MCP_SERVER_TOOL_TYPE_NUMBER; // 指令类型，AI通过这个类型发送相对应的数据
  rgb.inputSchema.properties[2].name = "green"; // 属性指令，AI 通过这个指令发送命令
  rgb.inputSchema.properties[2].description = "RGB彩灯的绿色,范围0-255,查询时为null"; // 指令描述，AI通过这个描述理解指令
  rgb.inputSchema.properties[2].type = MCP_SERVER_TOOL_TYPE_NUMBER; // 指令类型，AI通过这个类型发送相对应的数据
  rgb.inputSchema.properties[3].name = "blue"; // 属性指令，AI 通过这个指令发送命令
  rgb.inputSchema.properties[3].description = "RGB彩灯的蓝色,范围0-255,查询时为null"; // 指令描述，AI通过这个描述理解指令
  rgb.inputSchema.properties[3].type = MCP_SERVER_TOOL_TYPE_NUMBER; // 指令类型，AI通过这个类型发送相对应的数据
  rgb.setRequestHandler = emMCP_SetRGBHandler;   // 设置控制回调
  rgb.checkRequestHandler = emMCP_GetRGBHandler; // 设置查询回调

  emMCP_AddToolToToolList(&rgb); // 添加工具到工具列表
  emMCP_RegistrationTools();     // 注册工具到小安AI
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    emMCP_TickHandle(10);
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
