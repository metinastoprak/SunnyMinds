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


typedef enum _NEC_RxState{
    NEC_RX_STATE_IDLE,
    NEC_RX_STATE_INIT,
    NEC_RX_HEAD_OK,
    NEC_RX_HEAD_FAIL,
    NEC_RX_STATE_ERROR,
    NEC_RX_STATE_DONE,
    NEC_RX_STATE_MAX
}NEC_RxState;


typedef enum _NEC_TxState{
    NEC_TX_STATE_IDLE,
    NEC_TX_STATE_TRANSMIT,
    NEC_TX_STATE_FRAME_OK,
    NEC_TX_STATE_TRANSMIT_DONE
}NEC_TxState;

typedef enum _msgState{
    MSG_STATE_IDLE,
    MSG_STATE_READY,
    MSG_STATE_PROCESS,
}msgState;

typedef enum _sensor{
    SENSOR_1,
    SENSOR_2,
    SENSOR_MAX
}IRsensor;


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
