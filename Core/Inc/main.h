/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32l5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include  <string.h>
#include "stdint.h"
#include "stdbool.h"
#include "usbd_hid.h"
#include "usb_device.h"
#include "keyboard.h"
#include "is31fl3737.h"
#include "rgb.h"
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
#define col_13_Pin GPIO_PIN_13
#define col_13_GPIO_Port GPIOC
#define reset_radio_Pin GPIO_PIN_15
#define reset_radio_GPIO_Port GPIOC
#define col_0_Pin GPIO_PIN_0
#define col_0_GPIO_Port GPIOC
#define col_1_Pin GPIO_PIN_1
#define col_1_GPIO_Port GPIOC
#define col_2_Pin GPIO_PIN_2
#define col_2_GPIO_Port GPIOC
#define col_3_Pin GPIO_PIN_3
#define col_3_GPIO_Port GPIOC
#define row_0_Pin GPIO_PIN_0
#define row_0_GPIO_Port GPIOA
#define row_1_Pin GPIO_PIN_1
#define row_1_GPIO_Port GPIOA
#define row_2_Pin GPIO_PIN_2
#define row_2_GPIO_Port GPIOA
#define row_3_Pin GPIO_PIN_3
#define row_3_GPIO_Port GPIOA
#define row_4_Pin GPIO_PIN_4
#define row_4_GPIO_Port GPIOA
#define row_5_Pin GPIO_PIN_5
#define row_5_GPIO_Port GPIOA
#define col_4_Pin GPIO_PIN_4
#define col_4_GPIO_Port GPIOC
#define col_5_Pin GPIO_PIN_5
#define col_5_GPIO_Port GPIOC
#define matrix_Pin GPIO_PIN_12
#define matrix_GPIO_Port GPIOB
#define col_6_Pin GPIO_PIN_6
#define col_6_GPIO_Port GPIOC
#define col_7_Pin GPIO_PIN_7
#define col_7_GPIO_Port GPIOC
#define col_8_Pin GPIO_PIN_8
#define col_8_GPIO_Port GPIOC
#define col_9_Pin GPIO_PIN_9
#define col_9_GPIO_Port GPIOC
#define col_10_Pin GPIO_PIN_10
#define col_10_GPIO_Port GPIOC
#define col_11_Pin GPIO_PIN_11
#define col_11_GPIO_Port GPIOC
#define col_12_Pin GPIO_PIN_12
#define col_12_GPIO_Port GPIOC

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
