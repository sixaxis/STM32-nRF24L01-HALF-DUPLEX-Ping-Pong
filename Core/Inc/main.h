/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "nrf24.h"
#include "retarget.h"
#include  "stdio.h"

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
#define NRF_CE_Pin        GPIO_PIN_6
#define NRF_CE_GPIO_Port  GPIOB
#define NRF_CS_Pin        GPIO_PIN_7
#define NRF_CS_GPIO_Port  GPIOB
#define NRF_IRQ_Pin       GPIO_PIN_8
#define NRF_IRQ_GPIO_Port GPIOB
#define LED_PORT          GPIOA
#define LED_D2            GPIO_PIN_6
#define LED_D3            GPIO_PIN_7

#define LED_D3_ON  (HAL_GPIO_WritePin(LED_PORT, LED_D3, GPIO_PIN_RESET))
#define LED_D3_OFF (HAL_GPIO_WritePin(LED_PORT, LED_D3, GPIO_PIN_SET))

#define LED_D2_ON  (HAL_GPIO_WritePin(LED_PORT, LED_D2, GPIO_PIN_RESET))
#define LED_D2_OFF (HAL_GPIO_WritePin(LED_PORT, LED_D2, GPIO_PIN_SET))

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
