/*
 * app_common.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_COMMON_C

#include "main.h"




/*******************************************************************************
* LOCAL DEFINES
********************************************************************************/


/*******************************************************************************
* LOCAL TYPEDEFS 
********************************************************************************/


/*******************************************************************************
* LOCAL VARIABLES
********************************************************************************/


/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/



/*******************************************************************************
* FUNCTIONS
********************************************************************************/

/**
  * @brief EXTI line detection callbacks
  *  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  ULONG currentValue = 0;
  if (GPIO_Pin == USER_BUTTON_Pin)
  {
    /* Add additional checks to avoid multiple semaphore puts by successively
    clicking on the user button */
    tx_semaphore_info_get(&semaphore_ledcheck, NULL, &currentValue, NULL, NULL, NULL);
    //if (currentValue < 2)
    {
        /* Put the semaphore to release the MainThread */
         tx_semaphore_put(&semaphore_ledcheck);
    }
  }
}

/**
  * @brief  Application Delay function.
  *   Delay : number of ticks(thread) to wait (1tick=10ms)
  * @retval None
  */
void App_Delay(ULONG Delay)
{
  ULONG initial_time = tx_time_get();
  while ((tx_time_get() - initial_time) < Delay);
}



