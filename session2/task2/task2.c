
 /*************************************************** Task 2 *************************************************************/

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
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

void LED_Toggle100ms_Task(void * pvParameters);
void LED_Toggle500ms_Task(void * pvParameters);
void LED_Toggle1000ms_Task(void * pvParameters);

/*-----------------------------------------------------------*/

TaskHandle_t LED_100ms_TaskHandler =NULL;
TaskHandle_t LED_500ms_TaskHandler =NULL;
TaskHandle_t LED_1000ms_TaskHandler =NULL;

#define LED_100ms_TaskStack 100
#define LED_500ms_TaskStack 100
#define LED_1000ms_TaskStack 100

/*-----------------------------------------------------------*/

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
 
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();

	
    /* Create Tasks here */
	
	xTaskCreate(LED_Toggle100ms_Task,"LED_100ms Task",LED_100ms_TaskStack,NULL,0,&LED_100ms_TaskHandler);
  xTaskCreate(LED_Toggle500ms_Task,"LED_500ms Task",LED_500ms_TaskStack,NULL,0,&LED_500ms_TaskHandler);
	xTaskCreate(LED_Toggle1000ms_Task,"LED_1000ms Task",LED_1000ms_TaskStack,NULL,0,&LED_1000ms_TaskHandler);

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

void LED_Toggle100ms_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		
		pinState_t led1State = GPIO_read(PORT_0,PIN1);
		if(led1State== PIN_IS_LOW)
			{
				GPIO_write(PORT_0,PIN1,PIN_IS_HIGH);
			}
		else if(led1State== PIN_IS_HIGH)
			{
				 GPIO_write(PORT_0,PIN1,PIN_IS_LOW);
			}
			
			vTaskDelay(100);	/* periodicity : 100 ms */
	}
}

void LED_Toggle500ms_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		
		pinState_t led2State = GPIO_read(PORT_0,PIN2);
		if(led2State== PIN_IS_LOW)
			{
				GPIO_write(PORT_0,PIN2,PIN_IS_HIGH);
			}
		else if(led2State== PIN_IS_HIGH)
			{
				 GPIO_write(PORT_0,PIN2,PIN_IS_LOW);
			}
			
			vTaskDelay(500);	/* periodicity : 500 ms */
	}
}

void LED_Toggle1000ms_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		pinState_t led3State = GPIO_read(PORT_0,PIN3);
		if(led3State== PIN_IS_LOW)
			{
				GPIO_write(PORT_0,PIN3,PIN_IS_HIGH);
			}
		else if(led3State== PIN_IS_HIGH)
			{
				 GPIO_write(PORT_0,PIN3,PIN_IS_LOW);
			}
			
			vTaskDelay(1000);	/* periodicity : 1000 ms */
	}
}



/**********************************************************************************************************************/
