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
#include "stm32f4xx_hal_dma.h"
#include "stm32f4xx_hal_uart.h"
#include "system_stm32f4xx.h"
#include "uartPort.h"
#include "usart.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "emMCP.h"
#include "log.h"
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFFER_SIZE_MAX 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
emMCP_t emMCP;
emMCP_tool_t LED_Tools;
uint8_t rxBuffer[RXBUFFER_SIZE_MAX] = {0};
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if (huart == &huart2) {
    uartPortRecvData((char *)rxBuffer, Size);
    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, RXBUFFER_SIZE_MAX);
    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
  }
}

static void LED_ToolsRequestHandler(void *arg) {
  cJSON *param = (cJSON *)arg;
  cJSON *led1 = cJSON_GetObjectItem(param, "led1");
  cJSON *led2 = cJSON_GetObjectItem(param, "led2");
  cJSON *led3 = cJSON_GetObjectItem(param, "led3");
  cJSON *led4 = cJSON_GetObjectItem(param, "led4");
  if (led1 != NULL) {
    HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,
                      led1->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  if (led2 != NULL) {
    HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin,
                      led2->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  if (led3 != NULL) {
    HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin,
                      led3->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  if (led4 != NULL) {
    HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin,
                      led4->valueint ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
  emMCP_ResponseValue(emMCP_CTRL_OK);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick.
   */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  SystemInit();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, RXBUFFER_SIZE_MAX);
  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
  emMCP_Init(&emMCP); // Initialize MCP
  LED_Tools.name = "开发板LED";
  LED_Tools.description = "用于控制开发板的4颗LED灯";
  LED_Tools.inputSchema.properties[0].name = "led1";
  LED_Tools.inputSchema.properties[0].description =
      "控制LED1的亮灭,false为灭,true为亮";
  LED_Tools.inputSchema.properties[0].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
  LED_Tools.inputSchema.properties[1].name = "led2";
  LED_Tools.inputSchema.properties[1].description =
      "控制LED2的亮灭,false为灭,true为亮";
  LED_Tools.inputSchema.properties[1].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
  LED_Tools.inputSchema.properties[2].name = "led3";
  LED_Tools.inputSchema.properties[2].description =
      "控制LED3的亮灭,false为灭,true为亮";
  LED_Tools.inputSchema.properties[2].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;
  LED_Tools.inputSchema.properties[3].name = "led4";
  LED_Tools.inputSchema.properties[3].description =
      "控制LED4的亮灭,false为灭,true为亮";
  LED_Tools.inputSchema.properties[3].type = MCP_SERVER_TOOL_TYPE_BOOLEAN;

  LED_Tools.setRequestHandler = LED_ToolsRequestHandler;
  emMCP_AddToolToToolList(&LED_Tools);
  emMCP_RegistrationTools();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1) {
    /* USER CODE END WHILE */
    emMCP_TickHandle(100);
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

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
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
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n", file,
     line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
