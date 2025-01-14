/*
 * app_adc_test.h
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#ifndef APP_ADC_TEST_H
#define APP_ADC_TEST_H



#ifdef  APP_ADC_TEST_C
#define INTERFACE
#else 
#define INTERFACE	extern
#endif

//#include "tx_api.h"

/*******************************************************************************
* EXPORTED TYPES
********************************************************************************/


/*******************************************************************************
* EXPORTED DEFS
********************************************************************************/
#define ADC_CHANNEL_COUNT   8



/*******************************************************************************
* EXPORTED VARS
********************************************************************************/
INTERFACE uint16_t adc_result[ADC_CHANNEL_COUNT];

/*******************************************************************************
* EXPORTED FUNCTIONS
********************************************************************************/
INTERFACE void app_ADC_Init(void);
INTERFACE void app_ADC_DeInit(void);

INTERFACE void app_ADC_HSC_Test(void);
INTERFACE void adcTest_Callback(const char *subcommand, const char *args[], int argc);
INTERFACE void distanceTest_Callback(const char *subcommand, const char *args[], int argc);


#undef	INTERFACE
#endif  /* APP_ADC_TEST_H */
