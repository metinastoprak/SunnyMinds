/*
 * app_moduletest.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_MODULETEST_C

#include "main.h"
#include "app_moduletest.h"


//#include "app_netxduo.h"


/*******************************************************************************
* LOCAL DEFINES
********************************************************************************/
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len);
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */


/*******************************************************************************
* LOCAL TYPEDEFS 
********************************************************************************/

typedef struct _msgQueue {
    uint8_t id;
    uint8_t cmd;
    msgState state;
}t_msgQueue;

const char * Generic_State[6] =    {"IDLE", \
                                    "INIT", \
                                    "HEAD_OK", \
                                    "HEAD_FAIL", \
                                    "ERROR", \
                                    "DONE"};



/*******************************************************************************
* LOCAL VARIABLES
********************************************************************************/
TX_THREAD portal_ptr;

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/
VOID Portal_thread_entry(ULONG initial_param);


/*******************************************************************************
* FUNCTIONS
********************************************************************************/
/**
* @brief Function Name ModuleTest_Init()
*/
void ModuleTest_Init(void) {


}

  /**
  * @brief  ModuleTest thread entry function
  * @param  None
  * @retval None
  */
VOID Moduletest_thread_entry(ULONG initial_param){
    

    printf("[Thread-ModuleTEST] Entry\n\r");    
    while (1)
    {
        tx_thread_sleep(10);        //100ms sleep

        app_buttonLed_Test();
        app_qspi_Test();

/*
        switch(msgTransceiver.state)
        {
            case MSG_STATE_IDLE:
            {
                break;
            }
            case MSG_STATE_READY:
            {	
                break;
            }
            case MSG_STATE_PROCESS:
            {	
                break;
            }
        }
*/

    }

    
}

/**
  * @brief  Retargets the C library printf function to the USART.
  *   None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of putchar here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

