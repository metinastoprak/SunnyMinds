/*
 * app_cli.c
 * Command Line Interface
 * Read commands via UART (UDP etc) and execute
 *  Created on: Nov 18, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_CLI_C

#include "main.h"
#include <stdint.h>
#include <string.h>



/*******************************************************************************
* LOCAL DEFINES
********************************************************************************/
#ifndef INT_MIN
#define INT_MIN (-2147483647 - 1)
#endif
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#define BUFFER_SIZE             128             // Ring buffer lenght

#define UART_MSG_TERMINATOR '\n'    


void ledTest_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));
void flashMemTest_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));
void irTest_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));
void imuTest_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));
void adcTest_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));
void distanceTest_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));


void wifi_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));
void motor_Callback(const char *subcommand, const char *args[], int argc) __attribute__((weak));

/*******************************************************************************
* LOCAL TYPEDEFS 
********************************************************************************/
typedef enum _argCount{
    ARG_COUNT_NONE,
    ARG_COUNT_1,
    ARG_COUNT_2,
    ARG_COUNT_3,
    ARG_COUNT_4,
    ARG_COUNT_5,
    ARG_COUNT_6,
    ARG_COUNT_7,
    ARG_COUNT_8,
    ARG_COUNT_9,
    ARG_COUNT_10,
    ARG_COUNT_MAX=ARG_COUNT_10
}t_argCount;

typedef void (*CommandCallback)(const char *subcommand, const char *args[], int argc);

typedef struct {
    const char *name;       // Subcommand name
    const char **args;      // common args
    int maxArgcnt;          // possible arg count
}SubCommand;

// command config command - sub comamnds - ID
typedef struct {    
    const char *name;         
    SubCommand subcommands[MAX_SUBC_COUNT]; 
    int commandID;
    CommandCallback callback;                        
}CommandConfig;


// Parse command structure
typedef struct {
    int commandID;                                          // Komut kimliği
    char command[MAX_COMMAND_LENGTH];                       // Komut adı
    char subcommand[MAX_SUBCOMMAND_LENGTH];                 // Alt komut adı
    char args[MAX_ARGC_COUNT][MAX_SUBCOMMAND_LENGTH];          
    int argCount;                                           // arg count
} ParsedCommand;


const char * cmd_header ="SM=";
const char * cmd_LED_args[] = {"on", "off", "blink", NULL};
const char * cmd_IMU_args[] = {"ACC", "MG", NULL};
const char * cmd___args[] = {"start", "stop", NULL};

// command Table
const CommandConfig commandTable[] = {          // SM= cmd+subcmd + arg1+arg2+....arg10
    {"led", {                                   // LED 
        {"red",  cmd_LED_args,ARG_COUNT_1},  // sub command 
        {"green",cmd_LED_args,ARG_COUNT_1},
        {NULL,NULL,0}  
     }, 1,ledTest_Callback},
    {"flashmem", {                             // ex: flashmem testpages 2
        {"chiperase", NULL,ARG_COUNT_NONE},    // ex: flashmem chiperase  
        {"getID",NULL,ARG_COUNT_NONE},        
        {"testpages",  NULL,ARG_COUNT_1},       // flashmem testpages 10
        {"readpage",  NULL,ARG_COUNT_1},        // flashmem readpage 3
        {NULL,NULL,0}        
    }, 2,flashMemTest_Callback},
    {"ir", {                                  // ex: ir test
        {"test", NULL,ARG_COUNT_NONE},
        {NULL,NULL,0}
    }, 3,irTest_Callback},
    {"imu", {                                 // ex: imu read ACC
        {"read", cmd_IMU_args,ARG_COUNT_1},   // read ACC, read MG
        {NULL,NULL,0}
    }, 4,imuTest_Callback},
    {"adc", {                                 // ex: adc read (start or stop)
        {"read", cmd___args,ARG_COUNT_1},      
        {NULL,NULL,0}
    }, 5,adcTest_Callback},
    {"hcsr04", {                              // ex: adc read start or (stop)
        {"read", cmd___args,ARG_COUNT_1},      
        {NULL,NULL,0}
    }, 6,distanceTest_Callback},

    {"wifi", {                                  // WiFi
        {"connect", NULL,ARG_COUNT_1},          // ex: IP address 
        {"setparam",NULL,ARG_COUNT_2},        
        {"status",  NULL,ARG_COUNT_NONE},
        {NULL,NULL,0}        
    }, 7,wifi_Callback},
    {"motor", {                                 // Motor
        {"start", NULL,ARG_COUNT_NONE},
        {"stop",  NULL,ARG_COUNT_NONE},
        {"status",NULL,ARG_COUNT_NONE},
        {NULL,NULL,0}
    }, 8,motor_Callback},
    {NULL, {
        {NULL,NULL,ARG_COUNT_NONE} 
    }, 0,NULL}
};

// Ring buffer structure
typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint16_t head;
    uint16_t tail;
    uint16_t count;
} RingBuffer;


/*******************************************************************************
* LOCAL VARIABLES
********************************************************************************/
static RingBuffer serialBuffer;
static char message[BUFFER_SIZE];
static uint8_t tempbuff[2];


/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/
void RingBuffer_Init(RingBuffer *ringBuffer);
void RingBuffer_Write(RingBuffer *ringBuffer, uint8_t data);
int RingBuffer_Read(RingBuffer *ringBuffer, uint8_t *data);
int RingBuffer_ReadMessage(RingBuffer *ringBuffer, char *messageBuff, uint16_t maxLength);


int app_cli_parsecommand(ParsedCommand * pCommand,const char * msg);
int app_cli_verifycommand(ParsedCommand * pCommand);
int app_cli_executecommand(ParsedCommand * pCommand);

int app_cli_getNumber(const char *);

/*******************************************************************************
* FUNCTIONS
********************************************************************************/
/*
    Command syntax details
    ---> SM+"CMD"= "subCMD" "param1" "param2" "param3"....
         SM:SunnyMinds
         CMD: command name
         subCMD: sub command
         "paramX": given parameters

    ---> SM+"CMD"?
         response: returns command usage
         example: SM+led?
                  response: SM+led=red on   

*/
int app_cli_getNumber(const char * arg) {

    char *endptr;
    long number = strtol(arg, &endptr, 10);         // base 10

    // invalid char or not between limts
    if (*endptr != '\0' || number < INT_MIN || number > INT_MAX) {
        printf("[CLI-ERROR] '%s' invalid number!\r\n", arg);
        return INVALID_NUMBER;
    }

    return (int)number; // Başarılıysa tam sayı olarak döner
}
/**
* @brief xxxx_Callback
*  
* @retval 
*/
void ledTest_Callback(const char *subcommand, const char *args[], int argc) { }
void flashMemTest_Callback(const char *subcommand, const char *args[], int argc) { }
void irTest_Callback(const char *subcommand, const char *args[], int argc) { }
void imuTest_Callback(const char *subcommand, const char *args[], int argc) { }
void adcTest_Callback(const char *subcommand, const char *args[], int argc) { }
void distanceTest_Callback(const char *subcommand, const char *args[], int argc) {}

void wifi_Callback(const char *subcommand, const char *args[], int argc) {}
void motor_Callback(const char *subcommand, const char *args[], int argc) {}

/**
* @brief app_cli_executecommand
*  
* @retval status
*/
int app_cli_executecommand(ParsedCommand * pCommand) {

    // get command address  first
    CommandConfig * p_cmdTable = (CommandConfig *)&commandTable[pCommand->commandID-1];
    const char * argS[MAX_ARGC_COUNT];

    // copy argument(s)
    for (int i=0;i<pCommand->argCount;i++){
        argS[i] = pCommand->args[i];
    }

    // printout given command
    printf("[CLI-INFO] '%s %s",pCommand->command,pCommand->subcommand);
    for (int i=0;i<pCommand->argCount;i++){
        printf(" %s",pCommand->args[i]);
    }
    printf("' execution \r\n");
    p_cmdTable->callback(pCommand->subcommand,argS,pCommand->argCount);     //function call
    return 1;
}
/**
* @brief app_cli_parsecommand
*  
* @retval status
*/
int app_cli_verifycommand(ParsedCommand * pCommand) {

    CommandConfig * p_cmdTable = (CommandConfig *)commandTable;
    int index = 0;
    int argcount = 0;
    

    // validate command first
    while(p_cmdTable->name != NULL) {
        if (strcmp(p_cmdTable->name,pCommand->command) == 0)
        {
            pCommand->commandID = p_cmdTable->commandID;    // getID

            // validate sub command
             while(p_cmdTable->subcommands[index].name != NULL && (index < MAX_SUBC_COUNT)) {
                if (strcmp(p_cmdTable->subcommands[index].name,pCommand->subcommand) == 0)
                {   
                    // subc matched , validate arg counts & limits if exist
                    if (p_cmdTable->subcommands[index].maxArgcnt > ARG_COUNT_NONE) {
                        if (!pCommand->argCount) {
                            printf("[CLI-ERROR] argument missing(argcount=%d) \r\n",p_cmdTable->subcommands[index].maxArgcnt);return 0;
                        }
                        else if (pCommand->argCount > p_cmdTable->subcommands[index].maxArgcnt){
                            printf("[CLI-ERROR] more argument-%d \r\n",pCommand->argCount);return 0;
                        }
                    }
                    // valid args..
                    if (p_cmdTable->subcommands[index].args == NULL){
                        printf("[CLI-INFO] valid command \r\n");
                        return 1;
                    }

                    // args list avaliable ..Check list of defined params
                    if (p_cmdTable->subcommands[index].args != NULL){
                        for (int j = 0;p_cmdTable->subcommands[index].args[j] != NULL;j++) {
                            // scan all args ..
                            for (int k = 0;k < pCommand->argCount;k++)
                            {
                                if (strcmp(p_cmdTable->subcommands[index].args[j],pCommand->args[k]) == 0)
                                {   // args matched... 
                                    ++argcount;
                                }
                            }
                        }
                        if (p_cmdTable->subcommands[index].maxArgcnt > ARG_COUNT_NONE && argcount == pCommand->argCount){
                            printf("[CLI-INFO] command valid.. \r\n");
                            return 1;
                        }  
                        printf("[CLI-ERROR] invalid argument name\r\n");return 0;   
                    }
                    printf("[CLI-INFO] command valid.. \r\n");
                    return 1;
                }
                index++;
            }
            break;
        }
        p_cmdTable++;    
     }

    printf("[CLI-ERROR] invalid command or subcommand\r\n");
    return 0;
}
/**
* @brief app_cli_parsecommand
*  
* @retval status
*/
int app_cli_parsecommand(ParsedCommand * pCommand,const char * msg) {

    //CommandConfig *pCmdTable = (CommandConfig *)&commandTable[0];
    char * token = NULL;
    char * restmsg = NULL;                     // starts at 4.th character
    char strBuff[MAX_COMMAND_LENGTH];

 
    // check header part first..
    if (strncmp(cmd_header,msg,strlen(cmd_header)) == 0) {
        //printf("[CLI] SM+ heading passed\r\n");
        memset(strBuff,'\0',MAX_COMMAND_LENGTH);
        memset(pCommand->command,'\0',MAX_COMMAND_LENGTH);
        memset(pCommand->subcommand,'\0',MAX_SUBCOMMAND_LENGTH);


        // skip "SM+" head
        strncpy(strBuff,msg+strlen(cmd_header),(MAX_COMMAND_LENGTH-strlen(cmd_header)));

        // get main command
        restmsg = strBuff;
        token = strtok_r(restmsg, " ",&restmsg);
        if (token != NULL){
            strncpy(pCommand->command,token,MAX_COMMAND_LENGTH);   
            //printf("[CLI] command:%s \r\n",pCommand->command);            
        }
        else
        {
            printf("[CLI-ERROR] command missing \r\n");return 0;
        }
        // get subcommand
        token = strtok_r(restmsg, " ",&restmsg);
        if (token != NULL){
            strncpy(pCommand->subcommand,token,MAX_SUBCOMMAND_LENGTH);   
            //printf("[CLI] subcommand:%s \r\n",pCommand->subcommand);            
        }
        else
        {
            printf("[CLI-ERROR] subcommand missing\r\n");return 0;
        }


        // parse sub commandns & param
        int argCount = 0;
        while((token = strtok_r(NULL, " ",&restmsg)) != NULL && (argCount<MAX_ARGC_COUNT)) {
            // args parsing
            strncpy(pCommand->args[argCount],token,MAX_SUBCOMMAND_LENGTH);
            argCount++;  
        }
        if (token != NULL)
        {
            printf("[CLI-ERROR] error args > MAX_ARGC_COUNT(%d) \r\n",MAX_ARGC_COUNT);return 0;
        }
        pCommand->argCount = argCount;
        if (pCommand->argCount > 0) {
            for (int i=0;i<pCommand->argCount;i++);
                //printf("[CLI] args%d:%s \r\n",i+1,pCommand->args[i]);    
        }
        //printf("[CLI-INFO] message parsed succesfully\r\n");
        return 1;
    }
    
    printf("[CLI-ERROR] invalig Header->%s\r\n",cmd_header);
    return 0;               // invalid parse command
}

/**
* @brief app_cli_init
*  
* @retval None
*/
void app_cli_handler(void) {

//    ParsedCommand  parseCommand;
    ParsedCommand  parseCommand;

    if (RingBuffer_ReadMessage(&serialBuffer, message, BUFFER_SIZE)) {
        //printf("[CLI]-->message:%s\r\n",message);

        if (app_cli_parsecommand(&parseCommand,(const char *)message)){
 
             if (app_cli_verifycommand(&parseCommand)){
                // invoke command..
                app_cli_executecommand(&parseCommand);
             }
        }
    }


}

/**
* @brief app_cli_init
*  
* @retval None
*/
void app_cli_init(void) {

    RingBuffer_Init(&serialBuffer);
    HAL_UART_Receive_IT(&hlpuart1, tempbuff, (uint16_t)1);


}



/**
* @brief RingBuffer_Init
*  
* @retval None
*/
void RingBuffer_Init(RingBuffer *ringBuffer) {
    ringBuffer->head = 0;
    ringBuffer->tail = 0;
    ringBuffer->count = 0;
}

/**
* @brief RingBuffer_Write
*  
* @retval None
*/
void RingBuffer_Write(RingBuffer *ringBuffer, uint8_t data) {
    if (ringBuffer->count < BUFFER_SIZE) {
        ringBuffer->buffer[ringBuffer->head] = data;
        ringBuffer->head = (ringBuffer->head + 1) % BUFFER_SIZE;
        ringBuffer->count++;
    }
}

/**
* @brief RingBuffer_Read
*  
* @retval data, status
*/
int RingBuffer_Read(RingBuffer *ringBuffer, uint8_t *data) {
    if (ringBuffer->count > 0) {
        *data = ringBuffer->buffer[ringBuffer->tail];
        ringBuffer->tail = (ringBuffer->tail + 1) % BUFFER_SIZE;
        ringBuffer->count--;
        return 1;
    }
    return 0;
}
/**
* @brief RingBuffer_ReadMessage
*  
* @retval message, status
*/
int RingBuffer_ReadMessage(RingBuffer *ringBuffer, char *messageBuff, uint16_t maxLength) {
    uint16_t i = 0;
    uint8_t byte;

    while (RingBuffer_Read(ringBuffer, &byte)) {
        if (byte == UART_MSG_TERMINATOR) {
            messageBuff[i] = '\0';
            return 1;
        } else if (i < maxLength - 1) {
            messageBuff[i++] = byte;
        }
    }
    return 0;
}
/**
* @brief HAL_UART_RxCpltCallback
*  
* @retval RX data
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

    if (huart->Instance == LPUART1) {
        RingBuffer_Write(&serialBuffer, tempbuff[0]);
        HAL_UART_Receive_IT(&hlpuart1, tempbuff,(uint16_t) 1);
    }
}
