/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STATUS_LED_Pin GPIO_PIN_13
#define STATUS_LED_GPIO_Port GPIOC
#define RRST_Pin GPIO_PIN_0
#define RRST_GPIO_Port GPIOC
#define RBSY_Pin GPIO_PIN_2
#define RBSY_GPIO_Port GPIOC
#define RBSY_EXTI_IRQn EXTI2_IRQn
#define RDB0_Pin GPIO_PIN_0
#define RDB0_GPIO_Port GPIOA
#define RDB1_Pin GPIO_PIN_1
#define RDB1_GPIO_Port GPIOA
#define RDB2_Pin GPIO_PIN_2
#define RDB2_GPIO_Port GPIOA
#define RDB3_Pin GPIO_PIN_3
#define RDB3_GPIO_Port GPIOA
#define RDB4_Pin GPIO_PIN_4
#define RDB4_GPIO_Port GPIOA
#define RDB5_Pin GPIO_PIN_5
#define RDB5_GPIO_Port GPIOA
#define RDB6_Pin GPIO_PIN_6
#define RDB6_GPIO_Port GPIOA
#define RDB7_Pin GPIO_PIN_7
#define RDB7_GPIO_Port GPIOA
#define TDB0_Pin GPIO_PIN_0
#define TDB0_GPIO_Port GPIOB
#define TDB1_Pin GPIO_PIN_1
#define TDB1_GPIO_Port GPIOB
#define TDB2_Pin GPIO_PIN_2
#define TDB2_GPIO_Port GPIOB
#define TDBP_Pin GPIO_PIN_6
#define TDBP_GPIO_Port GPIOC
#define TSEL_Pin GPIO_PIN_7
#define TSEL_GPIO_Port GPIOC
#define TBSY_Pin GPIO_PIN_8
#define TBSY_GPIO_Port GPIOC
#define RDBP_Pin GPIO_PIN_9
#define RDBP_GPIO_Port GPIOC
#define RSEL_Pin GPIO_PIN_8
#define RSEL_GPIO_Port GPIOA
#define RACK_Pin GPIO_PIN_10
#define RACK_GPIO_Port GPIOA
#define RATN_Pin GPIO_PIN_13
#define RATN_GPIO_Port GPIOA
#define TIO_Pin GPIO_PIN_15
#define TIO_GPIO_Port GPIOA
#define TCD_Pin GPIO_PIN_10
#define TCD_GPIO_Port GPIOC
#define TMSG_Pin GPIO_PIN_11
#define TMSG_GPIO_Port GPIOC
#define TREQ_Pin GPIO_PIN_12
#define TREQ_GPIO_Port GPIOC
#define TDB3_Pin GPIO_PIN_3
#define TDB3_GPIO_Port GPIOB
#define TDB4_Pin GPIO_PIN_4
#define TDB4_GPIO_Port GPIOB
#define TDB5_Pin GPIO_PIN_5
#define TDB5_GPIO_Port GPIOB
#define TDB6_Pin GPIO_PIN_6
#define TDB6_GPIO_Port GPIOB
#define TDB7_Pin GPIO_PIN_7
#define TDB7_GPIO_Port GPIOB
#define SPI_RST_Pin GPIO_PIN_8
#define SPI_RST_GPIO_Port GPIOB
#define SPI_INT_Pin GPIO_PIN_9
#define SPI_INT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define MAX_PACKET_SIZE 600
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
