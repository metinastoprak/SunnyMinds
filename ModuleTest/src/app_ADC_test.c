/*
 * app_adc_test.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_ADC_TEST_C

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
uint16_t adcbuffer[ADC_CHANNEL_COUNT];
uint32_t adc_data[ADC_CHANNEL_COUNT];

uint8_t adc_counter;

static uint8_t read_lineSensor = 0;
static uint8_t read_distanceSensor = 0;

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/
float frontDistance(void);


/*******************************************************************************
* FUNCTIONS
********************************************************************************/
/**
* @brief 
*  
* @retval None
*/
void app_ADC_Init(void) {
       // ADC-DMA setting
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adcbuffer, 8);
    HAL_ADC_StartSampling(&hadc1);
    adc_counter = 0;
}
/**
* @brief 
*  
* @retval None
*/
void app_ADC_DeInit(void) {
       // ADC-DMA setting
    HAL_ADC_Stop_DMA(&hadc1);
    HAL_ADC_StopSampling(&hadc1);
}

/**
* @brief 
*  
* @retval None
*/
void app_ADC_HSC_Test(void) {

    if (read_lineSensor){
        
        for (int i=0;i<ADC_CHANNEL_COUNT;i++){
            float voltage_val = (adc_result[i] * 3.3)/4096;
            printf("[ch%d: %04d ->%1.2fv]  ",i+1,adc_result[i],voltage_val);
        }
        printf("\r\n");
    }
    if (read_distanceSensor) {
        float distance = frontDistance();
        printf("distance: %2.2f\r\n",distance);
    }
}
/**
* @brief adcTest_Callback
*  
* @retval
*/
void adcTest_Callback(const char *subcommand, const char *args[], int argc) {
    
    if (strcmp(subcommand,"read") == 0 && strcmp(args[0],"start") == 0){
        app_ADC_Init();
        read_lineSensor = 1;
        HAL_TIM_Base_Start(&htim16);
    }
    else if (strcmp(subcommand,"read") == 0 && strcmp(args[0],"stop") == 0){
        app_ADC_DeInit();
        read_lineSensor = 0;
    }
}
/**
  * @brief ADC Conv completed  callbacks
  *  reads #9 channels ADC
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
    if (hadc->Instance == ADC1) {

        for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
            adc_data[i] += adcbuffer[i];
        }
        adc_counter++;

        if (adc_counter >= 16) {
            for (int i = 0; i < ADC_CHANNEL_COUNT; i++) {
                adc_data[i] >>= 4;
                adc_result[i] = (uint16_t)adc_data[i];
                adc_data[i] = 0;
            }
            adc_counter = 0;
        }
    }
}
//=====================================================================0
/**
  * @brief frontDistance
  *  
  * @retval distance
  */
float frontDistance(void) {
    float distancefront;
    uint32_t Value2_3;
    uint32_t Value1_3;
    uint32_t pMillis3;

	HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_SET);  // pull the TRIG pin HIGH
    __HAL_TIM_SET_COUNTER(&htim16, 0);
    while (__HAL_TIM_GET_COUNTER (&htim16) < 10);  // wait for 10 us
    HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_RESET);  // pull the TRIG pin low

    pMillis3 = HAL_GetTick(); // used this to avoid infinite while loop  (for timeout)
    // wait for the echo pin to go high
    while (!(HAL_GPIO_ReadPin (ECHO1_GPIO_Port, ECHO1_Pin)) && pMillis3 + 10 >  HAL_GetTick());
    Value1_3 = __HAL_TIM_GET_COUNTER (&htim16);

    pMillis3 = HAL_GetTick(); // used this to avoid infinite while loop (for timeout)
    // wait for the echo pin to go low
    while ((HAL_GPIO_ReadPin (ECHO1_GPIO_Port, ECHO1_Pin)) && pMillis3 + 50 > HAL_GetTick());
    Value2_3 = __HAL_TIM_GET_COUNTER (&htim16);

    distancefront = (float) (Value2_3-Value1_3)* 0.034/2;
    HAL_Delay(50);

    return (distancefront);
}
/**
* @brief distanceTest_Callback
*  
* @retval
*/
void distanceTest_Callback(const char *subcommand, const char *args[], int argc) {
    if (strcmp(subcommand,"read") == 0 && strcmp(args[0],"start") == 0){
        read_distanceSensor = 1;
        HAL_TIM_Base_Start(&htim16);
    }
    else if (strcmp(subcommand,"read") == 0 && strcmp(args[0],"stop") == 0){
        read_distanceSensor = 0;
        HAL_TIM_Base_Stop(&htim16);
    }
}
