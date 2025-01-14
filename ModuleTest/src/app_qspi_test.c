/*
 * app_qspi_test.c
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#define	APP_QSPI_TEST_C

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
/*buffer that will be writtten n times to the external memory  */
unsigned char aTxBuffer[] ="**OCTOSPI/Quadspi SunnyMind Memorymapped communication example****OCTOSPI/Quadspi SunnyMind Memorymapped communication example**\
**OCTOSPI/Quadspi SunnyMind Memorymapped communication example****OCTOSPI/Quadspi SunnyMind Memorymapped communication example**";

__IO uint8_t *mem_addr;
static unsigned char aRxBuffer[256];

/* Exported macro -----------------------------------------------------*/
#define COUNTOF(__BUFFER__) (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
#define BUFFERSIZE (COUNTOF(aTxBuffer) - 1)

#define PAGE_TEST_COUNT  500     // to be tested.. Normally: 4096

/*******************************************************************************
* LOCAL FUNCTION PROTOTYPES
********************************************************************************/


void app_qspi_ResetChip(void);
void app_qspi_ChipERASE(void);

void app_qspi_ReadDeviceID(void);
void app_qspi_WriteEnable(void);
void app_qspi_WriteDisable(void);
void app_qspi_ReadPage(uint16_t pageCount);

uint8_t app_qspi_WriteMemoryPAGE(uint8_t* buffer, uint32_t address);
uint8_t app_qspi_ReadMemoryPAGE(uint8_t* buffer, uint32_t address);

uint8_t app_qspi_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t app_qspi_AutoPollingMemReady(void);
uint8_t app_qspi_EnableMemoryMappedMode(void);


/*******************************************************************************
* FUNCTIONS
********************************************************************************/
/**
* @brief app_qspi_Init
*  
* @retval None
*/
void app_qspi_Init(void) {
    app_qspi_ResetChip();
    app_qspi_ChipERASE();
    app_qspi_ReadDeviceID();
}
/**
* @brief flashMemTest_Callback
*  
* @retval
*/
void flashMemTest_Callback(const char *subcommand, const char *args[], int argc){
    //printf("[LED] 'led %s' command execution\r\n", subcommand);
    char command[MAX_COMMAND_LENGTH]={0};

    // add Subcommand
    strncat(command, subcommand, sizeof(command) - 1);

    // add Args
    for (int i = 0; i < argc; i++){
        strncat(command, " ", sizeof(command) - strlen(command) - 1); // Boşluk ekle
        strncat(command, args[i], sizeof(command) - strlen(command) - 1);
    }

    if (strcmp(command,"getID") == 0){
        app_qspi_ReadDeviceID();
    }
    else if(strcmp(command,"chiperase") == 0){
        app_qspi_ChipERASE();
    }
    else if(strcmp(subcommand,"testpages") == 0){
        int arg = app_cli_getNumber(args[0]);
        if (arg != INVALID_NUMBER && arg >= 0){
            app_qspi_Test(arg);             
        }
        else
            printf("[QSPI-ERROR] invalid number '%d' \r\n",arg);    
    }
    else if(strcmp(subcommand,"readpage") == 0){
        int arg = app_cli_getNumber(args[0]);
        if (arg != INVALID_NUMBER && arg >= 0){
            app_qspi_ReadPage(arg);             
        }    
        else
            printf("[QSPI-ERROR] invalid number '%d' \r\n",arg);    
    }

}
/**
* @brief app_qspi_Test
*  
* @retval None
*/
void app_qspi_Test(uint16_t pageCount) {

    unsigned int i;

    for (i = 0; i < pageCount; i++) {
        // write test pattern chars
        if (app_qspi_WriteMemoryPAGE(aTxBuffer,i*MEMORY_PAGE_SIZE) != HAL_OK) {
            printf("[QSPI-ERROR] WritePAGE Error at %d \r\n",i);       
        }
    }
    HAL_Delay(250);
    printf("\r\n[QSPI-INFO] Test Pattern writing success\r\n");
    // Verify Patterns
    for (i = 0; i < pageCount; i++) {

        // clear buffer before reading device     
        memset(aRxBuffer, 0, sizeof(aRxBuffer));    
        if (app_qspi_ReadMemoryPAGE(aRxBuffer,i*MEMORY_PAGE_SIZE) != HAL_OK) {
            printf("[QSPI-ERROR] Page READ ERROR at page:%d.. \r\n",i);       
        }
        // compare PAGE
        if (memcmp(aTxBuffer,aRxBuffer,MEMORY_PAGE_SIZE) != HAL_OK) {
            printf("[QSPI-ERROR] Pattern Verificaiton ERROR at page:%d.. \r\n",i);       
        }
        HAL_Delay(1);
    }
    printf("[QSPI-INFO] Test Pattern verification SUCCESS!..\r\n");
}
/**
* @brief app_qspi_ReadPage
*  
* @retval None
*/
void app_qspi_ReadPage(uint16_t pageCount){

    memset(aRxBuffer, 0, sizeof(aRxBuffer));    
    if (app_qspi_ReadMemoryPAGE(&aRxBuffer[0],pageCount*MEMORY_PAGE_SIZE) != HAL_OK) {
        printf("[QSPI-ERROR] Page READ ERROR at page:%d.. \r\n",pageCount);       
    }
    printf("[QSPI-INFO] page'%d'->%s \r\n",pageCount,aRxBuffer);       
}
/**
* @brief QUADSPI Write Memory-Page (256 Byte)
*  
* @retval Status
*/

uint8_t app_qspi_WriteMemoryPAGE(uint8_t* buffer, uint32_t address) {

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_PAGEPROGRAM,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .Address = address,
        .AddressMode = HAL_XSPI_ADDRESS_1_LINE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
 		.DataMode = HAL_XSPI_DATA_1_LINE,
        .DummyCycles = 0,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG,
        .DataLength = MEMORY_PAGE_SIZE
    };
    app_qspi_WriteEnable();

    /* Send Config */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }
    /* Transmission of the data */
    if (HAL_XSPI_Transmit(&hospi1, buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {

        return HAL_ERROR;
    }


    if (app_qspi_AutoPollingMemReady() != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}
/**
* @brief QUADSPI Write Memory-Page (256 Byte)
*  
* @retval Status
*/
uint8_t app_qspi_ReadMemoryPAGE(uint8_t* buffer, uint32_t address) {

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_FASTREAD,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .Address = address,
        .AddressMode = HAL_XSPI_ADDRESS_1_LINE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
 		.DataMode = HAL_XSPI_DATA_1_LINE,

        .DummyCycles = FAST_READ_QUAD_DUMMY_CYCLES,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG,
        .DataLength = MEMORY_PAGE_SIZE,
    };

    /* Send Config */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }
    /* Transmission of the data */
    if (HAL_XSPI_Receive(&hospi1, buffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {

        return HAL_ERROR;
    }

    return HAL_OK;
}
/**
* @brief QUADSPI Erase Memory sectors
*  
* @retval Status
*/
uint8_t app_qspi_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress) {

    EraseStartAddress = EraseStartAddress - EraseStartAddress % MEMORY_SECTOR_SIZE;

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_BLOCKERASE_4K,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .AddressMode = HAL_XSPI_ADDRESS_1_LINE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
 		.DataMode = HAL_XSPI_DATA_NONE,
        .DummyCycles = 0,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };
    /* Send Erase 4K Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    while (EraseEndAddress >= EraseStartAddress) {
        sCommand.Address = (EraseStartAddress & 0x00FFFFFF);

        app_qspi_WriteEnable();

        if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
            return HAL_ERROR;
        }
        EraseStartAddress += MEMORY_SECTOR_SIZE;

        if (app_qspi_AutoPollingMemReady() != HAL_OK) {
            return HAL_ERROR;
        }
    }

    return HAL_OK;
}
/**
* @brief QUADSPI AutoPolling Memory process
*  
* @retval Status
*/
uint8_t app_qspi_AutoPollingMemReady(void)
{
    XSPI_AutoPollingTypeDef sConfig = {
    .MatchValue = 0x00,
    .MatchMask = 0x01,
    .MatchMode = HAL_XSPI_MATCH_MODE_AND,
    .IntervalTime = 0x10,
    .AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE
    };

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_READSTATUSREGS,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .AddressMode = HAL_XSPI_ADDRESS_NONE,
 		.DataMode = HAL_XSPI_DATA_1_LINE,
        .DummyCycles = 0,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };

    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }

    if (HAL_XSPI_AutoPolling(&hospi1, &sConfig,HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK) {
        return HAL_ERROR;
    }
    return HAL_OK;
}
/**
* @brief QUADSPI ResetChip
*  
* @retval None
*/
void app_qspi_ResetChip(void)
{
    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_RESET_ENABLE,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .AddressMode = HAL_XSPI_ADDRESS_NONE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
 		.DataMode = HAL_XSPI_DATA_NONE,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };
    /* Send Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] Chip RESET, OSPI init failed. \r\n");return;
    }
    sCommand.Instruction = QSPI_CMD_RESET_DEVICE;

    /* Send Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] Chip RESET sending command failed. \r\n");
    }
    else
    {
        printf("[QSPI-INFO] Chip RESET done.. \r\n");
        HAL_Delay(10);
    }
}
/**
* @brief QUADSPI Chip Erase
*  
* @retval None
*/
void app_qspi_ChipERASE(void)
{
    app_qspi_WriteEnable();

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_CHIPERASE1,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .AddressMode = HAL_XSPI_ADDRESS_NONE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
 		.DataMode = HAL_XSPI_DATA_NONE,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };
    /* Send Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] Chip ERASE, OSPI init failed. \r\n");return;
    }
    
    if (app_qspi_AutoPollingMemReady() != HAL_OK) {
        printf("[QSPI-ERROR] Chip Erasing failed. \r\n");
    }
    else
    {
        printf("[QSPI-INFO] Chip Erasing DONE!.. \r\n");
        HAL_Delay(250);    
    }
}
/**
* @brief QUADSPI WriteEnable
*  
* @retval None
*/
void app_qspi_WriteEnable(void)
{

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_WRITEENABLE,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .AddressMode = HAL_XSPI_ADDRESS_NONE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,

 		.DataMode = HAL_XSPI_DATA_NONE,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };

    /* Send Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] WRITE_ENABLE failed. \r\n");
    }
    HAL_Delay(10);
}
/**
* @brief QUADSPI WriteDisable
*  
* @retval None
*/
void app_qspi_WriteDisable(void)
{

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_WRITEDISABLE,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .AddressMode = HAL_XSPI_ADDRESS_NONE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,

 		.DataMode = HAL_XSPI_DATA_NONE,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };

    /* Send Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] WRITE_DISABLE failed. \r\n");
    }
    HAL_Delay(10);
}

/**
* @brief QUADSPI Enable ReadDeviceID
*  
* @retval None
*/
void app_qspi_ReadDeviceID(void)
{

    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_JEDECID,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
        .InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS,

        .AddressMode = HAL_XSPI_ADDRESS_NONE,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
 		.DataMode = HAL_XSPI_DATA_1_LINE,

 //       .AlternateBytes = 3U,
 //       .AlternateBytesMode = HAL_XSPI_ALT_BYTES_1_LINE,
 //       .AlternateBytesWidth = HAL_XSPI_ALT_BYTES_8_BITS,

		.DataLength = 3U,
		.DummyCycles = 0U,
        .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG
    };

    /* Send Comamnd */
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] SPI command failed. \r\n");
    }
    /* Read JEDEC ID via Quad Read mode 1-0-1*/ 
    if (HAL_XSPI_Receive(&hospi1,aRxBuffer, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        printf("[QSPI-ERROR] Read device ID failed. \r\n");
    }
    else
    {
        printf("[QSPI-INFO] ManufactID:0x%02X DeviceID-1:0x%02X DeviceID-2:0x%02X \r\n",aRxBuffer[0],aRxBuffer[1],aRxBuffer[2]); 
    }
    HAL_Delay(10);
}


/**
* @brief QUADSPI Enable MemoryMapped Mode in Quad 1-1-4
*  
* @retval None
*/
uint8_t app_qspi_EnableMemoryMappedMode(void)
{
    XSPI_MemoryMappedTypeDef sMemMappedCfg;

    // Quad Output Fast Read 1-1-4
    XSPI_RegularCmdTypeDef sCommand = {
		.Instruction = QSPI_CMD_QUADFASTREAD,
		.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,

        .AddressMode = HAL_XSPI_DATA_4_LINES,
        .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,

 		.DataMode = HAL_XSPI_DATA_4_LINES,

		.DummyCycles = 4   //FAST_READ_QUAD_DUMMY_CYCLES,
//        .OperationType = HAL_XSPI_OPTYPE_READ_CFG
    };

    /* Memory-mapped mode configuration for Quad Read mode 1-4-4*/ 
    if (HAL_XSPI_Command(&hospi1, &sCommand, HAL_XSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }


    /*Disable timeout counter for memory mapped mode*/
    sMemMappedCfg.TimeOutActivation = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
    /*Enable memory mapped mode*/
    if (HAL_XSPI_MemoryMapped(&hospi1, &sMemMappedCfg) != HAL_OK)
    {
        return HAL_ERROR;
    } 
    return HAL_OK;
}

#if 0
/**
* @brief QUADSPI init function
*  
* @retval None
*/

uint8_t CSP_QUADSPI_Init(void) {

    //prepare QSPI peripheral for ST-Link Utility operations
	hospi1.Instance = QUADSPI;
    
    if (HAL_QSPI_DeInit(&hqspi) != HAL_OK) {
        return HAL_ERROR;
    }

    MX_QUADSPI_Init();

    if (QSPI_ResetChip() != HAL_OK) {
        return HAL_ERROR;
    }

    HAL_Delay(1);

    if (QSPI_AutoPollingMemReady() != HAL_OK) {
        return HAL_ERROR;
    }

    if (QSPI_WriteEnable() != HAL_OK) {

        return HAL_ERROR;
    }

    if (QSPI_Configuration() != HAL_OK) {
        return HAL_ERROR;
    }

    return HAL_OK;
}
#endif
