/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "app_threadx.h"

#include "app_moduletest.h"
#include "app_common.h"
#include "app_buttonled_test.h"
#include "app_qspi_test.h"
#include "app_ir_test.h"
#include "app_imu_test.h"





/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern UART_HandleTypeDef hlpuart1;
extern XSPI_HandleTypeDef hospi1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim17;
extern I2C_HandleTypeDef hi2c2;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern void Error_Handler(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define USER_LED2_Pin GPIO_PIN_3
#define USER_LED2_GPIO_Port GPIOE
#define INT1_LSM6D_Pin GPIO_PIN_2
#define INT1_LSM6D_GPIO_Port GPIOF
#define INT2_LSM6D_Pin GPIO_PIN_3
#define INT2_LSM6D_GPIO_Port GPIOF
#define SA0_LSM6D_Pin GPIO_PIN_4
#define SA0_LSM6D_GPIO_Port GPIOF
#define IR_RX_Pin GPIO_PIN_5
#define IR_RX_GPIO_Port GPIOA
#define TRIG1_Pin GPIO_PIN_7
#define TRIG1_GPIO_Port GPIOE
#define USER_LED1_Pin GPIO_PIN_15
#define USER_LED1_GPIO_Port GPIOB
#define IR_TX_Pin GPIO_PIN_5
#define IR_TX_GPIO_Port GPIOB
#define USER_BUTTON_Pin GPIO_PIN_0
#define USER_BUTTON_GPIO_Port GPIOE
#define USER_BUTTON_EXTI_IRQn EXTI0_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
