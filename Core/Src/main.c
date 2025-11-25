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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  // Enable DWT Cycle Counter for precise microsecond delay
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t ticks_per_us = HAL_RCC_GetHCLKFreq() / 1000000;
  uint32_t cycle_start_tick = DWT->CYCCNT;

  // Ensure Cam_enable is set
  HAL_GPIO_WritePin(Cam_enable_GPIO_Port, Cam_enable_Pin, GPIO_PIN_SET);

  while (1) {
    uint32_t current_tick = DWT->CYCCNT;
    uint32_t elapsed_ticks = current_tick - cycle_start_tick;
    uint32_t elapsed_us = elapsed_ticks / ticks_per_us;

    // 1 second cycle (1,000,000 us)
    if (elapsed_us >= 1000000) {
      cycle_start_tick = current_tick;
      elapsed_us = 0;
      elapsed_ticks = 0; // Reset for safety
    }

    // --- Task 1: Push-Pull (Wan/Tui) ---
    // Active for first 200ms (200,000 us)
    if (elapsed_us < 200000) {
      // 1000Hz signal -> 1000us period
      uint32_t sub_cycle_us = elapsed_us % 1000;

      // Dead time 10us, Active 490us, Dead time 10us, Active 490us
      if (sub_cycle_us < 10) {
        // Dead time 1 (0-10us)
        HAL_GPIO_WritePin(Wan_GPIO_Port, Wan_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Tui_GPIO_Port, Tui_Pin, GPIO_PIN_RESET);
      } else if (sub_cycle_us < 500) {
        // State 1: Wan High (10-500us)
        HAL_GPIO_WritePin(Wan_GPIO_Port, Wan_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Tui_GPIO_Port, Tui_Pin, GPIO_PIN_RESET);
      } else if (sub_cycle_us < 510) {
        // Dead time 2 (500-510us)
        HAL_GPIO_WritePin(Wan_GPIO_Port, Wan_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Tui_GPIO_Port, Tui_Pin, GPIO_PIN_RESET);
      } else {
        // State 2: Tui High (510-1000us)
        HAL_GPIO_WritePin(Wan_GPIO_Port, Wan_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Tui_GPIO_Port, Tui_Pin, GPIO_PIN_SET);
      }
    } else {
      // Inactive period (200ms - 1000ms)
      HAL_GPIO_WritePin(Wan_GPIO_Port, Wan_Pin, GPIO_PIN_RESET);
      HAL_GPIO_WritePin(Tui_GPIO_Port, Tui_Pin, GPIO_PIN_RESET);
    }

    // --- Task 2: Camera Shoot ---
    // Trigger at 50ms (50,000 us) for 5ms (5,000 us)
    if (elapsed_us >= 50000 && elapsed_us < 55000) {
      HAL_GPIO_WritePin(shoot_GPIO_Port, shoot_Pin, GPIO_PIN_SET);
    } else {
      HAL_GPIO_WritePin(shoot_GPIO_Port, shoot_Pin, GPIO_PIN_RESET);
    }
  }
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
    Error_Handler();
  }
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, Cam_enable_Pin | Wan_Pin | Tui_Pin | shoot_Pin,
                    GPIO_PIN_RESET);

  /*Configure GPIO pins : Cam_enable_Pin Wan_Pin Tui_Pin shoot_Pin */
  GPIO_InitStruct.Pin = Cam_enable_Pin | Wan_Pin | Tui_Pin | shoot_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
