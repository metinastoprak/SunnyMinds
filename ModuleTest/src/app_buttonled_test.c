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

    
    static char buttonPressed = 0;

    if (!buttonPressed && tx_semaphore_get(&semaphore_buttonpress, TX_NO_WAIT) == TX_SUCCESS)
    {
        buttonPressed = 1; 
        tx_semaphore_get(&semaphore_buttonpress, TX_NO_WAIT);   
        printf("\r\n[BUTTON] user button pressed, LEDs flashes.. \r\n");
    }
    else if (buttonPressed) {
        HAL_GPIO_TogglePin(USER_LED1_GPIO_Port,USER_LED1_Pin);
        HAL_GPIO_TogglePin(USER_LED2_GPIO_Port,USER_LED2_Pin);

        if (HAL_GPIO_ReadPin(USER_BUTTON_GPIO_Port,USER_BUTTON_Pin) == 0) {
            HAL_GPIO_WritePin(USER_LED1_GPIO_Port,USER_LED1_Pin,GPIO_PIN_RESET);
            HAL_GPIO_WritePin(USER_LED2_GPIO_Port,USER_LED2_Pin,GPIO_PIN_RESET);
            buttonPressed = 0;
            tx_semaphore_get(&semaphore_buttonpress, TX_NO_WAIT); 
        }
    }
}
/**
* @brief ledTest_Callback
*  
* @retval
*/
void ledTest_Callback(const char *subcommand, const char *args[], int argc) {
    //printf("[LED] 'led %s' command execution\r\n", subcommand);
    char command[MAX_COMMAND_LENGTH]={0};
 
    // add Subcommand
    strncat(command, subcommand, sizeof(command) - 1);

    tx_thread_suspend(&thread_Led1);
    tx_thread_suspend(&thread_Led2);

    // add Args
    for (int i = 0; i < argc; i++) {
        strncat(command, " ", sizeof(command) - strlen(command) - 1); // Boşluk ekle
        strncat(command, args[i], sizeof(command) - strlen(command) - 1);
    }

    if (strcmp(command,"red off") == 0){
        HAL_GPIO_WritePin(USER_LED1_GPIO_Port,USER_LED1_Pin,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(USER_LED2_GPIO_Port,USER_LED2_Pin,GPIO_PIN_RESET);
    }
    else if(strcmp(command,"red on") == 0){
        HAL_GPIO_WritePin(USER_LED1_GPIO_Port,USER_LED1_Pin,GPIO_PIN_SET);
        HAL_GPIO_WritePin(USER_LED2_GPIO_Port,USER_LED2_Pin,GPIO_PIN_SET);
    }
    else if(strcmp(command,"red blink") == 0){
        tx_thread_resume(&thread_Led1);
        tx_thread_resume(&thread_Led2);
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
