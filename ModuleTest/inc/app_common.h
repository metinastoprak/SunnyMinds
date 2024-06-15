/*
 * app_common.h
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#ifndef APP_COMMON_H
#define APP_COMMON_H



#ifdef  APP_COMMON_C
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


#define     TICK_PER_MS  10        
#define     TICK_100_MS  (100/TICK_PER_MS) 
#define     TICK_500_MS  (500/TICK_PER_MS) 
#define     TICK_1_SEC   (1000/TICK_PER_MS) 
#define     TICK_2_SEC   (2000/TICK_PER_MS) 
#define     TICK_5_SEC   (5000/TICK_PER_MS) 



/*******************************************************************************
* EXPORTED VARS
********************************************************************************/

/*******************************************************************************
* EXPORTED FUNCTIONS
********************************************************************************/
INTERFACE void App_Delay(ULONG);



#undef	INTERFACE
#endif  /* APP_MODULETEST_H */
