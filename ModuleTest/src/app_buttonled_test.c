/*
 * app_buttonled_test.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_BUTTONLED_TEST_C

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
void app_buttonLed_Test(void) {

    ULONG currentValue = 0;
    UINT state;
    char * name;

    if (tx_semaphore_get(&semaphore_ledcheck, TX_NO_WAIT) == TX_SUCCESS)
    {
        name = "LED1 Thread";
        tx_semaphore_info_get(&semaphore_ledcheck, NULL, &currentValue, NULL, NULL, NULL);
        printf("\r\n[BUTTON-LED] user button pressed\r\n");

        if (tx_thread_info_get(&thread_Led1,&name,&state,NULL,NULL,NULL,NULL,NULL,NULL) == TX_SUCCESS) {

            if (state == TX_SUSPENDED){
                // resume LEDx threads    
                printf("[BUTTON-LED] LEDs blinking\r\n");
                tx_thread_resume(&thread_Led1);
                tx_thread_resume(&thread_Led2);
            
            }
            else{
                // test completed, suspend LEDx threads
                tx_thread_suspend(&thread_Led1);
                tx_thread_suspend(&thread_Led2);
                HAL_GPIO_WritePin(USER_LED1_GPIO_Port,USER_LED1_Pin,GPIO_PIN_RESET);
                HAL_GPIO_WritePin(USER_LED2_GPIO_Port,USER_LED2_Pin,GPIO_PIN_RESET);
                printf("[BUTTON-LED] turnoff LEDs \r\n");    
            }
        }
    }
}

/**
 * @brief  LED1_thread_entry function
 * @param  None
 * @retval None
*/
VOID LED1_thread_entry(ULONG initial_param){
    

    printf("[Thread-LED1] Entry\r\n");    
    while (1)
    {
        tx_thread_sleep(100);        //100ms sleep
        HAL_GPIO_TogglePin(USER_LED1_GPIO_Port,USER_LED1_Pin);
    }
}
/**
 * @brief  LED2_thread_entry function
 * @param  None
 * @retval None
*/
VOID LED2_thread_entry(ULONG initial_param){
    
    printf("[Thread-LED2] Entry\r\n");    
    while (1)
    {
        tx_thread_sleep(200);        //100ms sleep
        HAL_GPIO_TogglePin(USER_LED2_GPIO_Port,USER_LED2_Pin);
    }
}
