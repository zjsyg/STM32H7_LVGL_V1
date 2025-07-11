/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

#include "stm32h7xx_ll_system.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_exti.h"
#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_cortex.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_utils.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_dma.h"

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
#define KEY_Pin LL_GPIO_PIN_13
#define KEY_GPIO_Port GPIOC
#define KEY_EXTI_IRQn EXTI15_10_IRQn
#define LED_R_Pin LL_GPIO_PIN_0
#define LED_R_GPIO_Port GPIOC
#define LED_G_Pin LL_GPIO_PIN_1
#define LED_G_GPIO_Port GPIOC
#define LED_B_Pin LL_GPIO_PIN_2
#define LED_B_GPIO_Port GPIOC
#define LCD_BL_Pin LL_GPIO_PIN_3
#define LCD_BL_GPIO_Port GPIOD
#define LCD_RST_Pin LL_GPIO_PIN_4
#define LCD_RST_GPIO_Port GPIOD
#define LCD_DC_Pin LL_GPIO_PIN_4
#define LCD_DC_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
