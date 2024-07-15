/*
 * app_IR_test.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_IR_TEST_C

#include "main.h"




/*******************************************************************************
* LOCAL DEFINES
********************************************************************************/
#define NEC_TIME_HEAD   13500
#define NEC_TIME_BIT0   1125
#define NEC_TIME_BIT1   2250
#define NEC_TIME_GAP    44000


#define NEC_TIME_BIT1_BOUNDARY_HI   (NEC_TIME_BIT1+(NEC_TIME_BIT1>>2))
#define NEC_TIME_BIT1_BOUNDARY_LO   (NEC_TIME_BIT1-(NEC_TIME_BIT1>>2))
#define NEC_TIME_BIT0_BOUNDARY_HI   (NEC_TIME_BIT0+(NEC_TIME_BIT0>>2))
#define NEC_TIME_BIT0_BOUNDARY_LO   (NEC_TIME_BIT0-(NEC_TIME_BIT0>>2))

#define NEC_PERIOD_US   26          // usec  38Khz modulation
#define NEC_PULSE_HEAD  346
#define NEC_PULSE_BIT   22

#define NEC_MARK_HEAD   (NEC_PULSE_HEAD*NEC_PERIOD_US)
#define NEC_SPACE_HEAD  (NEC_TIME_HEAD - NEC_MARK_HEAD)

#define NEC_MARK_BIT    (NEC_PULSE_BIT*NEC_PERIOD_US)
#define NEC_SPACE_BIT0  (NEC_TIME_BIT0 - NEC_MARK_BIT)
#define NEC_SPACE_BIT1  (NEC_TIME_BIT1 - NEC_MARK_BIT)



#define NEC_BIT_LENGHT      (16+16)
#define NEC_RAW_DATALENGHT  (NEC_BIT_LENGHT<<1)


#define PWM_STOP()      HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
#define PWM_START()     HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

#define IRMODUL_TRIAL_COUNT 3
/*******************************************************************************
* LOCAL TYPEDEFS 
********************************************************************************/
typedef struct {
    uint32_t rawTimerData[NEC_BIT_LENGHT+2];
    uint8_t decoded[4];

    NEC_RxState state;
    TIM_HandleTypeDef *timerHandle;

    uint32_t timerChannel;
    HAL_TIM_ActiveChannel timerChannelActive;

    uint8_t timeout;

    uint16_t timingHEADboundary;
    uint16_t timingBITboundary;

    uint16_t address;
    uint8_t command;

    void (*Rx_DecodedCallback)(void);
    void (*Rx_ErrorCallback)(void);
    void (*Rx_RepeatCallback)(void);
}t_NEC_RX;

typedef struct _t_NEC_TX {
    

    NEC_TxState state;
    TIM_HandleTypeDef *timerHandle;

    uint32_t timerChannel;
    HAL_TIM_ActiveChannel timerChannelActive;
    uint8_t timeout;

    uint8_t repeatCNT;

    uint8_t address;
    uint8_t command;
    uint32_t rawBitData;
    uint32_t encodedData;


    uint8_t bitCount;
    uint16_t markspace[(NEC_BIT_LENGHT+2)<<1];

    void (*Tx_XmitCallback)();
}t_NEC_TX;

// states
const char * NEC_rx_State[NEC_RX_STATE_MAX] = {" IDLE", \
                                                "INIT", \
                                                "HEAD_OK", \
                                                "HEAD_FAIL", \
                                                "ERROR", \
                                                "DONE"};



/*******************************************************************************
* LOCAL VARIABLES
********************************************************************************/
t_NEC_RX NEC_rx;
t_NEC_TX NEC_tx;

static t_testState IR_test_state = TEST_STATE_IDLE;
static uint8_t IR_test_timeout = 0;
static uint8_t IR_test_repeat = 0;

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/
// NEC -TX Protocol Functions 
void NEC_TX_Init(t_NEC_TX* handle);
uint8_t NEC_TX_ReverseLSB(uint8_t data);

// callback functions
void NEC_TX_XmitHandler(t_NEC_TX* handle);
void NEC_TX_SendMarkSpace(void);

// NEC -RX Protocol Functions 
void NEC_RX_Init(t_NEC_RX* handle);
void NEC_RX_StartCapture(t_NEC_RX* handle);
void NEC_RX_StopCapture(t_NEC_RX* handle) ;
void NEC_RX_TIM_IC_CaptureCallback(t_NEC_RX *handle);

// callback functions
void NEC_RX_DecodeHandler(void);
void NEC_RX_ErrorHandler(void);
void NEC_RX_RepeatHandler(void);



/*******************************************************************************
* FUNCTIONS
********************************************************************************/

/**
* @brief IR TX-RX Module Test
*  
* @retval None
*/
void app_IR_Test(void) {

    switch(IR_test_state)
    {
        case TEST_STATE_IDLE:
        {
            PWM_STOP(); 
            NEC_TX_Init(&NEC_tx);
            NEC_RX_Init(&NEC_rx);
            IR_test_state = TEST_STATE_PROCESS;
            NEC_RX_StartCapture(&NEC_rx);

            break;
        }
        case TEST_STATE_PROCESS:
        {	
            if (NEC_tx.state == NEC_TX_STATE_IDLE)
            {
                //NEC_RX_StartCapture(&NEC_rx[SENSOR_1]);        // disable receive
                NEC_tx.address = 0xA5+IR_test_repeat;  
                NEC_tx.command = CMD_TEST;
                NEC_TX_XmitHandler(&NEC_tx);
                printf("[IRMODUL-TX] %d. trial, transmitting --> ID:0x%X , Cmd:0x%X\r\n",IR_test_repeat+1,NEC_tx.address,NEC_tx.command);
            }
            else if (NEC_tx.state == NEC_TX_STATE_TRANSMIT_DONE) {
                if (NEC_rx.state == NEC_RX_STATE_DONE ){
                    printf("[IRMODUL-RX] frame captured Address:0x%X , Command:0x%X\r\n",NEC_rx.address,NEC_rx.command);
                    IR_test_state = TEST_STATE_PASSED;
                }

            }
            if (NEC_rx.state == NEC_RX_HEAD_OK) {
                
                if (NEC_rx.state != NEC_RX_STATE_INIT) {
                    if (++NEC_rx.timeout >= TICK_1_SEC/10)
                    {
                        if (NEC_tx.state == NEC_TX_STATE_TRANSMIT_DONE) {
                            IR_test_state = TEST_STATE_FAILED;    
                        }
                        else {
                            NEC_rx.Rx_ErrorCallback();
                        }    
                    } 
                }
            }
            if (++IR_test_timeout >= TICK_5_SEC/10)
            {
                
                printf("\r[IRMODUL-TX-RX] Error,signal receive timeout\n");
                if (IR_test_repeat < IRMODUL_TRIAL_COUNT-1)
                    IR_test_state = TEST_STATE_PASSED;
                else
                    IR_test_state = TEST_STATE_FAILED;
            }        
            break;
        }
        case TEST_STATE_PASSED:
        {
            if (IR_test_repeat < IRMODUL_TRIAL_COUNT-1){
                App_Delay(25);         // wait 0.25sec
                IR_test_timeout = 0;
                IR_test_repeat++;
                IR_test_state = TEST_STATE_IDLE;
            }
            else
            {
                printf("\r[IRMODUL-TX-RX] Test Completed Successfully..\r\n");
                IR_test_state = TEST_STATE_DONE;
            }

            break;
        }
       case TEST_STATE_FAILED:
        {
            printf("\r[IRMODUL-TX-RX] Test Failed,Press RESET button to try again !...\n");

            IR_test_state = TEST_STATE_DONE;
            PWM_STOP();    
            break;
        }
       case TEST_STATE_DONE:
        {
            NEC_RX_StopCapture(&NEC_rx);        
            PWM_STOP();

            break;
        }
 
    }


}
/**
 * @brief  NEC protocol decoded
 * @param  None
 * @retval None
 */
void NEC_RX_DecodeHandler(void) {

    //printf("\r[IRMODUL-RX] frame captured Address:0x%X , Command:0x%X\n ",NEC_rx.address,NEC_rx.command);
 }
/**
 * @brief  NEC protocol error handler
 * @param  None
 * @retval None
 */
void NEC_RX_ErrorHandler(void) {

    printf("\r[IRMODUL-RX] Timeout or Error handled! \n ");
    if (NEC_tx.state == NEC_TX_STATE_TRANSMIT_DONE){
        IR_test_state = TEST_STATE_FAILED;return;
    }

    tx_thread_sleep(10);
    NEC_RX_StartCapture(&NEC_rx);
}
/**
 * @brief  NEC protocol repeat state handler
 * @param  None
 * @retval None
 */
void NEC_RX_RepeatHandler(void) {

    printf("[IRMODUL-RX] repeat signal \r\n ");
    tx_thread_sleep(10);
    NEC_RX_StartCapture(&NEC_rx);
}

//***** NEC RX InputCapture Driver Functions *****// 
/**
 * @brief  NEC-RX initialize
 * @param  None
 * @retval None
 */
void NEC_RX_Init(t_NEC_RX* handle) {

    handle->timerHandle = &htim2;
    handle->timerChannel = TIM_CHANNEL_1;
    handle->timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_1;

    handle->timingBITboundary = (NEC_TIME_BIT0+NEC_TIME_BIT1)>>1;    //   1125 >---boundary---< 2250
    handle->timingHEADboundary = NEC_TIME_HEAD>>2;

    handle->Rx_DecodedCallback = NEC_RX_DecodeHandler;
    handle->Rx_ErrorCallback = NEC_RX_ErrorHandler;
    handle->Rx_RepeatCallback = NEC_RX_RepeatHandler;

    if (IR_test_repeat == 0)
        printf("[IRMODUL-RX] state_init with TIM2_CH1\r\n");
}
/**
 * @brief  NEC-RX init state and start to capture signals
 * @param  None
 * @retval None
 */
void NEC_RX_StartCapture(t_NEC_RX* handle) {

    handle->state = NEC_RX_STATE_INIT;

    handle->timeout = 0;
    handle->decoded[0]=0;handle->decoded[1]=0;
    handle->decoded[2]=0;handle->decoded[3]=0;
    HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);
    HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
            (uint32_t*) handle->rawTimerData, 4);

    //printf("\r[IRMODUL-RX] waiting to capture IR signal..\n ");
}
/**
 * @brief  NEC-RX de-init state and stop capture signals
 * @param  None
 * @retval None
 */
void NEC_RX_StopCapture(t_NEC_RX* handle) {

    handle->state = NEC_RX_STATE_IDLE;

    handle->timeout = 0;
    handle->decoded[0]=0;handle->decoded[1]=0;
    handle->decoded[2]=0;handle->decoded[3]=0;
    HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);

    //printf("\r[IRMODUL-RX] STOP capture IR signal\n ");
}
/**
 * @brief  NEC-RX HAL_TIM_IC_CaptureCallback
 * @param  None
 * @retval None
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {
            NEC_RX_TIM_IC_CaptureCallback(&NEC_rx);
    }
}
/**
 * @brief  Input Capture  Callback function
 * @param  None
 * @retval None
 */
void NEC_RX_TIM_IC_CaptureCallback(t_NEC_RX *handle)
{

    uint16_t diffTime;
     
    handle->timeout = 0;
    if (handle->state == NEC_RX_STATE_INIT) {

        HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);
        diffTime = handle->rawTimerData[1] - handle->rawTimerData[0];
 
        if ( ((NEC_TIME_HEAD + handle->timingHEADboundary) > diffTime) && (diffTime > (NEC_TIME_HEAD - handle->timingHEADboundary)) ) {
            // Head sync captured
            handle->state = NEC_RX_HEAD_OK;
            HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
                    (uint32_t*) handle->rawTimerData+2, NEC_RAW_DATALENGHT);

        } 
        else {  // no sync - recapture
            handle->state = NEC_RX_STATE_INIT;
            HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
                    (uint32_t*) handle->rawTimerData, 4);
        }

    } else if (handle->state == NEC_RX_HEAD_OK) {

        HAL_TIM_IC_Stop_DMA(handle->timerHandle, handle->timerChannel);

        for (int pos = 0; pos < NEC_BIT_LENGHT; pos++) {
            diffTime = handle->rawTimerData[pos+2] - handle->rawTimerData[pos+1];
            if ( (NEC_TIME_BIT1_BOUNDARY_HI > diffTime) && (NEC_TIME_BIT1_BOUNDARY_LO < diffTime) ) {
                handle->decoded[pos / 8] |= 1 << (pos % 8);
            } 
            else if ( (NEC_TIME_BIT0_BOUNDARY_HI > diffTime) && (NEC_TIME_BIT0_BOUNDARY_LO < diffTime) ) {
                handle->decoded[pos / 8] &= ~(1 << (pos % 8));

            }
            else {   // bit timing error
                handle->Rx_ErrorCallback();return;
            }
            handle->rawTimerData[pos] = diffTime; 
        }


        if ( (handle->decoded[2] ^ handle->decoded[3]) == 0xFF)
        {   // data matched
            handle->address = (handle->decoded[0]<<8)|handle->decoded[1];
            handle->command = handle->decoded[2];
            handle->state = NEC_RX_STATE_DONE;
            HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
            (uint32_t*) handle->rawTimerData, 2);

            handle->Rx_DecodedCallback();
        }
        else {
            handle->Rx_ErrorCallback();    
        }
    } else if (handle->state == NEC_RX_STATE_DONE) {
        HAL_TIM_IC_Start_DMA(handle->timerHandle, handle->timerChannel,
                (uint32_t*) handle->rawTimerData, 2);

    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  NEC-TX initialize
 * @param  None
 * @retval None
 */
void NEC_TX_Init(t_NEC_TX* handle) {
    handle->timerHandle = &htim3;

    handle->timerChannel = TIM_CHANNEL_2;
    handle->timerChannelActive = HAL_TIM_ACTIVE_CHANNEL_2;

    handle->Tx_XmitCallback = NEC_TX_SendMarkSpace;
    handle->state = NEC_TX_STATE_IDLE;

    if (IR_test_repeat == 0)
        printf("\r\n[IRMODUL-TX] state_init with TIM3_CH2\n");
}
/**
 * @brief  NEC-TX NEC_TX_XmitHandler
 * @param  address(8bit) command(8bit)
 * @retval None
 */
void NEC_TX_XmitHandler(t_NEC_TX* handle) {

    uint8_t cnt;
    handle->rawBitData = NEC_TX_ReverseLSB(handle->address)<<24;
    handle->rawBitData |= NEC_TX_ReverseLSB(~handle->address)<<16;
    handle->rawBitData |= NEC_TX_ReverseLSB(handle->command)<<8;
    handle->rawBitData |= NEC_TX_ReverseLSB(~handle->command);

    handle->encodedData = handle->rawBitData;       // backup 32bit data

    handle->markspace[0] = NEC_MARK_HEAD;
    handle->markspace[1] = NEC_SPACE_HEAD;

    for(cnt=0;cnt<(NEC_BIT_LENGHT<<1);cnt++)
    {
        handle->markspace[cnt+2] = NEC_MARK_BIT;
        cnt++;
        if(handle->rawBitData & 0x80000000)
            handle->markspace[cnt+2] = NEC_SPACE_BIT1;    // logic1 Mark
        else
            handle->markspace[cnt+2] = NEC_SPACE_BIT0;

        handle->rawBitData <<=1;
    }
    // last mark
    handle->markspace[cnt+2] = NEC_MARK_BIT;
    handle->markspace[cnt+3] = NEC_TIME_GAP;
    for (cnt=0;cnt<5;cnt++)
    {
        handle->state = NEC_TX_STATE_TRANSMIT;
        handle->bitCount = 0;

//        handle->Tx_XmitCallback();
    if (__HAL_TIM_GET_IT_SOURCE(&htim17,TIM_IT_UPDATE) == RESET ){
        __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,9);
        PWM_STOP();//HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);

        __HAL_TIM_SET_COUNTER(&htim17,0);
        __HAL_TIM_SET_AUTORELOAD(&htim17,44000);
        HAL_TIM_Base_Start_IT(&htim17);
    }

        while (handle->state != NEC_TX_STATE_FRAME_OK);
    }

    handle->state = NEC_TX_STATE_TRANSMIT_DONE;
}
/**
 * @brief  NEC-TX NEC_TX_SendMarkSpace
 * @param  None
 * @retval None
 */
void NEC_TX_SendMarkSpace(void) {
    
    while(HAL_GPIO_ReadPin(IR_TX_GPIO_Port,IR_TX_Pin) == GPIO_PIN_SET);
    PWM_STOP();//HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);

    

    __HAL_TIM_SET_AUTORELOAD(&htim17,NEC_tx.markspace[NEC_tx.bitCount]);
    __HAL_TIM_SET_COUNTER(&htim17,0);

    if (NEC_tx.bitCount & 0x01){ // mark or space ?
        PWM_STOP(); //HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
    }
    else {
        PWM_START();//HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    }

    if(++NEC_tx.bitCount >= (NEC_BIT_LENGHT+2)<<1)
    {
        HAL_TIM_Base_Stop_IT(&htim17);
        PWM_STOP();//HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
        NEC_tx.state = NEC_TX_STATE_FRAME_OK;
    }

}
/**
 * @brief  NEC-TX NEC_TX_ReverseLSB
 * @param  data(8bit)
 * @retval reversed byte
 */
uint8_t NEC_TX_ReverseLSB(uint8_t data) {

    uint8_t LSBfirst =0;
    for(uint8_t indx=0;indx<8;indx++)
    {
        LSBfirst |= (data & 0x01)<<(7-indx);
        data >>= 1;
    }
    return LSBfirst;
}

