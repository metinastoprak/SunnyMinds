/*
 * app_qspi_test.h
 *
 *  Created on: June 01, 2024
 *      Author: SunnyTeknoloji
 */

#ifndef APP_QSPI_TEST_H
#define APP_QSPI_TEST_H



#ifdef  APP_QSPI_TEST_C
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
/* AT25SF081B memory parameters*/
#define MEMORY_FLASH_SIZE               0x100000    /* 8 MBits - 1MB*/
#define MEMORY_SECTOR_SIZE              0x1000      /* 4kBytes */
#define MEMORY_PAGE_SIZE                0x100       /* 256 bytes */


/* AT25SF081B  commands */
#define QSPI_CMD_READSFDP           0x5A
#define QSPI_CMD_ID                 0x90
#define QSPI_CMD_JEDECID            0x9F
#define QSPI_CMD_UNIQUEID           0x4B

#define QSPI_CMD_READSTATUSREGS     0x05
#define QSPI_CMD_READSTATUS2        0x35

#define QSPI_CMD_WRITEENABLE        0x06
#define QSPI_CMD_WRITESTATUSEN      0x50
#define QSPI_CMD_WRITEDISABLE       0x04

#define QSPI_CMD_PAGEPROGRAM        0x02    // 1-1-1
#define QSPI_CMD_QUADPAGEPROGRAM    0x32    // 1-1-4

#define QSPI_CMD_WRITESTATUS1       0x01
#define QSPI_CMD_WRITESTATUS2       0x31

#define QSPI_CMD_ADDR4BYTE_EN       0xB7
#define QSPI_CMD_ADDR4BYTE_DIS      0xE9
#define QSPI_CMD_PAGEPROG4ADD       0x12

#define QSPI_CMD_NORMALREAD         0x03    // 1-1-1
#define QSPI_CMD_FASTREAD           0x0B    // 1-1-1  8cycle
#define QSPI_CMD_DUALIOFASTREAD     0xBB    // 1-2-2  , 0-2-2  y ModeBitClock:4  
#define QSPI_CMD_QUADOUTFASTREAD    0x6B    // 1-1-4  8cycle
#define QSPI_CMD_QUADFASTREAD       0xEB    // 1-4-4  , 0-4-4  4cycle Dummy  


#define QSPI_CMD_BLOCKERASE_4K      0x20    // 4Kbyte
#define QSPI_CMD_BLOCKERASE_32K     0x52    // 32Kbyte
#define QSPI_CMD_BLOCKERASE_64K     0xD8    // 64Kbyte
#define QSPI_CMD_CHIPERASE1         0x60
#define QSPI_CMD_CHIPERASE2         0xC7


#define QSPI_CMD_SUSPEND            0x75
#define QSPI_CMD_RESUME             0x7A
#define QSPI_CMD_FRAMSERNO          0xC3

#define QSPI_CMD_RESET_ENABLE       0x66
#define QSPI_CMD_RESET_DEVICE       0x99
#define QSPI_CMD_POWERDOWN          0xB9
#define QSPI_CMD_RELEASE            0xAB


#define DUMMY_CLOCK_CYCLES_READ_QUAD    10
#define FAST_READ_QUAD_DUMMY_CYCLES     8


/*******************************************************************************
* EXPORTED VARS
********************************************************************************/

/*******************************************************************************
* EXPORTED FUNCTIONS
********************************************************************************/
INTERFACE void app_qspi_Test(void);




#undef	INTERFACE
#endif  /* APP_QSPI_TEST_H */
