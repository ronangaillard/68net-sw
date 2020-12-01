/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "helpers.h"
#include "scsi.h"
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

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */

  // Init SCSI transmit pins
  HAL_GPIO_WritePin(TDB0_GPIO_Port, TDB0_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB1_GPIO_Port, TDB1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB2_GPIO_Port, TDB2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB3_GPIO_Port, TDB3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB4_GPIO_Port, TDB4_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB5_GPIO_Port, TDB5_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB6_GPIO_Port, TDB6_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDB7_GPIO_Port, TDB7_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TDBP_GPIO_Port, TDBP_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(TREQ_GPIO_Port, TREQ_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TMSG_GPIO_Port, TMSG_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TCD_GPIO_Port, TCD_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TIO_GPIO_Port, TIO_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TBSY_GPIO_Port, TBSY_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(TSEL_GPIO_Port, TSEL_Pin, GPIO_PIN_RESET);

  HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint8_t requestId = 0;
  char buffer[100];
  uint8_t status;
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // Wait for reset
    HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_SET);
    while (RSEL() || RBSY());

    requestId = RDB0_GPIO_Port->IDR;
    if ((~requestId & SCSI_ADDRESS) == SCSI_ADDRESS) {
      HAL_GPIO_WritePin(STATUS_LED_GPIO_Port, STATUS_LED_Pin, GPIO_PIN_RESET);
      TBSY(GPIO_PIN_SET);
      // Wait for SEL inactive
      LOG("SELECTED\n");
      while (!RSEL());

      if (!RATN()) {
        LOG("RATN OK\n");
      }

      TCD(GPIO_PIN_SET);

      uint8_t cmd[12];

      cmd[0] = readHandshake();
      cmd[1] = readHandshake();
      cmd[2] = readHandshake();
      cmd[3] = readHandshake();
      cmd[4] = readHandshake();
      cmd[5] = readHandshake();

      static const int cmd_class_len[8] = {6, 10, 10, 6, 6, 12, 6, 6};
      int len = cmd_class_len[cmd[0] >> 5];

      for (int i = 6; i < len; i++) {
        cmd[i] = readHandshake();
      }
      sprintf(buffer, "CMD : 0x%02X\n", cmd[0]);
      LOG(buffer);


      switch (cmd[0]) {
        case 0x12:
          LOG("[Inquiry]\n");
          status = onInquiryCommand(cmd[4]);
          break;
        default:
          LOG("UNDEFINED CMD\n");
      }

      // Status
      TCD(GPIO_PIN_SET);
      writeHandshake(status);

      // Message In
      TMSG(GPIO_PIN_SET);
      writeHandshake(0);

      TCD(GPIO_PIN_RESET);
      TIO(GPIO_PIN_RESET);
      TMSG(GPIO_PIN_RESET);
      TBSY(GPIO_PIN_RESET);

      LOG("BUS FREE\n");
    }


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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, STATUS_LED_Pin | TDBP_Pin | TSEL_Pin | TBSY_Pin
                           | TCD_Pin | TMSG_Pin | TREQ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TDB0_Pin | TDB1_Pin | TDB2_Pin | SPI_CS_Pin
                           | TDB3_Pin | TDB4_Pin | TDB5_Pin | TDB6_Pin
                           | TDB7_Pin | SPI_RST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TIO_GPIO_Port, TIO_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : STATUS_LED_Pin TDBP_Pin TSEL_Pin TBSY_Pin
                           TCD_Pin TMSG_Pin TREQ_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_Pin | TDBP_Pin | TSEL_Pin | TBSY_Pin
                        | TCD_Pin | TMSG_Pin | TREQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RRST_Pin RBSY_Pin RDBP_Pin */
  GPIO_InitStruct.Pin = RRST_Pin | RBSY_Pin | RDBP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : RDB0_Pin RDB1_Pin RDB2_Pin RDB3_Pin
                           RDB4_Pin RDB5_Pin RDB6_Pin RDB7_Pin
                           RSEL_Pin PA9 RACK_Pin RATN_Pin */
  GPIO_InitStruct.Pin = RDB0_Pin | RDB1_Pin | RDB2_Pin | RDB3_Pin
                        | RDB4_Pin | RDB5_Pin | RDB6_Pin | RDB7_Pin
                        | RSEL_Pin | GPIO_PIN_9 | RACK_Pin | RATN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TDB0_Pin TDB1_Pin TDB2_Pin SPI_CS_Pin
                           TDB3_Pin TDB4_Pin TDB5_Pin TDB6_Pin
                           TDB7_Pin SPI_RST_Pin */
  GPIO_InitStruct.Pin = TDB0_Pin | TDB1_Pin | TDB2_Pin | SPI_CS_Pin
                        | TDB3_Pin | TDB4_Pin | TDB5_Pin | TDB6_Pin
                        | TDB7_Pin | SPI_RST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TIO_Pin */
  GPIO_InitStruct.Pin = TIO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(TIO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_INT_Pin */
  GPIO_InitStruct.Pin = SPI_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI_INT_GPIO_Port, &GPIO_InitStruct);

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

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
