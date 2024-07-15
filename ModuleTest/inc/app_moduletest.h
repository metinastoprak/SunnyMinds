/*
 * app_moduletest.h
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#ifndef APP_MODULETEST_H
#define APP_MODULETEST_H



#ifdef  APP_MODULETEST_C
#define INTERFACE
#else 
#define INTERFACE	extern
#endif


/*******************************************************************************
* EXPORTED TYPES
********************************************************************************/


/*******************************************************************************
* EXPORTED DEFS
********************************************************************************/
#define TX_APP_MODULE_STACK_SIZE      1024
#define TX_APP_LEDCHECK_STACK_SIZE    512

#define MODULETEST_PRIORITY           12           // module test thread priority


/*
#define QUEUE_STACK_SIZE        32*4
#define TRANSCEIVER_PRIORITY    8
#define PORTAL_PRIORITY         9

#define QUEUE_SIZE              (20<<2)
#define MESSAGE_SIZE            5           // 5*32bit = 20char
*/
typedef enum _testState{
    TEST_STATE_IDLE,
    TEST_STATE_PROCESS,
    TEST_STATE_PASSED,
    TEST_STATE_FAILED,
    TEST_STATE_DONE
}t_testState;


/*******************************************************************************
* EXPORTED VARS
********************************************************************************/
INTERFACE TX_THREAD thread_moduletest;
INTERFACE TX_THREAD thread_Led1;
INTERFACE TX_THREAD thread_Led2;

INTERFACE TX_SEMAPHORE semaphore_ledcheck;


/*******************************************************************************
* EXPORTED FUNCTIONS
********************************************************************************/
INTERFACE VOID Moduletest_thread_entry(ULONG initial_param);
INTERFACE VOID LED1_thread_entry(ULONG initial_param);
INTERFACE VOID LED2_thread_entry(ULONG initial_param);



#undef	INTERFACE
#endif  /* APP_COMMON_H */
