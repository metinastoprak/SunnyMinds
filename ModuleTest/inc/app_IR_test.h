/*
 * app_buttonled_test.h
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#ifndef APP_IR_TEST_H
#define APP_IR_TEST_H



#ifdef  APP_IR_TEST_C
#define INTERFACE
#else 
#define INTERFACE	extern
#endif

//#include "tx_api.h"

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

#define     ASSIGN_TO_PATTERN(value) (uint16_t)(((value & 0xFF) << 8) | ((~value) & 0xFF))

    /* commands ID for communication between STMStation module */
#define     CMD_READY   0x0B
#define     CMD_START   0x0C
#define     CMD_FINISH  0x0D
#define     CMD_TEST    0x23

#define     CMD_ACK     0x10
#define     CMD_NACK    0xF0
#define     CMD_NULL    0x00

#define     CMD_ADR_ALL 0xFF      //send 0xFF00      // received command will be processed by all robovehicle ID



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
INTERFACE void app_IR_Test(void);

//INTERFACE void NEC_TX_SendMarkSpace(void);



#undef	INTERFACE
#endif  /* APP_IR_TEST_H */
