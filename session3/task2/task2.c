
/*
	try different priority levels
	preemption enable from config file
	understand the effect of the load
*/
 /*************************************************** Task 2 *************************************************************/

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "lpc21xx.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*-----------------------------------------------------------*/

/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */

static void prvSetupHardware( void );

void UART_100ms(void * pvParameters);
void UART_500ms(void * pvParameters);


/*-----------------------------------------------------------*/

TaskHandle_t UART100msHandler =NULL;
TaskHandle_t UART500msHandler =NULL;
SemaphoreHandle_t xMutex =NULL;

#define UART100msStack 100
#define UART500msStack 100



/*-----------------------------------------------------------*/

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
 
 uint32_t i=0;
 uint32_t j=0;
 uint32_t load ,delay;
 
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    /* Create Tasks here */
	xTaskCreate(UART_100ms,"UART 100ms Task",UART100msStack,NULL,2,&UART100msHandler);
	xTaskCreate(UART_500ms,"UART 500ms Task",UART500msStack,NULL,1,&UART500msHandler);
	
	/* Create the mutex */
	xMutex = xSemaphoreCreateMutex();

	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */

	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}

/*-----------------------------------------------------------*/
void UART_100ms(void * pvParameters)
{
	TickType_t xLastWakeTime1;
	xLastWakeTime1 = xTaskGetTickCount();
	for( ; ; ) 
	{
		vTaskDelayUntil( &xLastWakeTime1, 100 );
		if( xMutex != NULL )
		{
			if( xSemaphoreTake( xMutex, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
			{
				for(i=0;i<10;i++)
				{
					delay=0;
					vSerialPutString((const signed char *)"UART1\n",6);
					while(delay<2000){delay++;} /*delay  to fix an issue caused by vSerialPutString */
				}
				xSemaphoreGive( xMutex );
			}
		}
	}
}


void UART_500ms(void * pvParameters)
{
	TickType_t xLastWakeTime2;
	xLastWakeTime2 = xTaskGetTickCount();
	for( ; ; ) 
	{
		vTaskDelayUntil( &xLastWakeTime2, 500 );
		if( xMutex != NULL )
		{
			if( xSemaphoreTake( xMutex, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
			{
				for(j=0;j<10;j++)
				{
					load=0;
					vSerialPutString((const signed char *)"UART5\n",6);

					while(load<100000){load++;}
				}
				xSemaphoreGive( xMutex );
			}
		}
	}
}


/**********************************************************************************************************************/




