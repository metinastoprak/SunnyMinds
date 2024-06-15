/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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

/* Includes ------------------------------------------------------------------*/
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_moduletest.h"

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
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  CHAR *pointer;

  /* Allocate the stack for Main Thread  */
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_MODULE_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */
  if (tx_thread_create(&thread_moduletest, "ModuleTest Thread", Moduletest_thread_entry, 0, pointer,
                       TX_APP_MODULE_STACK_SIZE, MODULETEST_PRIORITY, MODULETEST_PRIORITY,
                       TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  // LED1 thread create
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_LEDCHECK_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  if (tx_thread_create(&thread_Led1, "LED1 Thread", LED1_thread_entry, 0, pointer,
                       TX_APP_LEDCHECK_STACK_SIZE, MODULETEST_PRIORITY, MODULETEST_PRIORITY,
                       TX_NO_TIME_SLICE, TX_DONT_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  // LED2 thread create
  if (tx_byte_allocate(byte_pool, (VOID**) &pointer,
                       TX_APP_LEDCHECK_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }
  if (tx_thread_create(&thread_Led2, "LED2 Thread", LED2_thread_entry, 0, pointer,
                       TX_APP_LEDCHECK_STACK_SIZE, MODULETEST_PRIORITY, MODULETEST_PRIORITY,
                       TX_NO_TIME_SLICE, TX_DONT_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Create Semaphore.  */
  if (tx_semaphore_create(&semaphore_ledcheck, "LED Test", 0) != TX_SUCCESS)
  {
    return TX_SEMAPHORE_ERROR;
  }



  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
