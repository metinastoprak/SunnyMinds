/*
 * app_cli.h
 *
 *  Created on: Nov 18, 2024
 *      Author: SunnyTeknoloji
 */

#ifndef APP_CLI_H
#define APP_CLI_H



#ifdef  APP_CLI_C
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
#define MAX_COMMAND_LENGTH      64   
#define MAX_SUBCOMMAND_LENGTH   16   

#define MAX_SUBC_COUNT          10        
#define MAX_ARGC_COUNT          10  
#define INVALID_NUMBER          -1



/*******************************************************************************
* EXPORTED VARS
********************************************************************************/

/*******************************************************************************
* EXPORTED FUNCTIONS
********************************************************************************/
INTERFACE void app_cli_init(void);
INTERFACE void app_cli_handler(void);
INTERFACE int app_cli_getNumber(const char *);


#undef	INTERFACE
#endif  /* APP_CLI_H */
