
 /*************************************************** Task 1 *************************************************************/

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
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

void Button_Task(void * pvParameters);
void LED_Task(void * pvParameters);


/*-----------------------------------------------------------*/

TaskHandle_t ButtonTaskHandler =NULL;
TaskHandle_t LED_TaskHandler =NULL;
SemaphoreHandle_t xSemaphore;

#define ButtonTaskStack 100
#define LED_TaskStack 100



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
	xSemaphore = xSemaphoreCreateBinary();
	xTaskCreate(Button_Task,"Button Task",ButtonTaskStack,NULL,2,&ButtonTaskHandler);
	xTaskCreate(LED_Task,"LED Task",LED_TaskStack,NULL,1,&LED_TaskHandler);


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


pinState_t LEDState =0x00;
typedef enum{
	initial_state,
	pressed,
	released
}buttonStates;

buttonStates buttonState =initial_state;
uint8_t b_read;

/*-----------------------------------------------------------*/
void Button_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		/* assume gpio pin to be pull-up which means 
		 * when the button is pressed it reads 0
		 * when the button is released/floatng it reads 1
		*/
		b_read= GPIO_read(PORT_0,PIN0);
		
		if(b_read ==PIN_IS_LOW) /*pressed*/
		{
			buttonState=pressed; 
		}
		else if(b_read ==PIN_IS_HIGH && buttonState==pressed) /* released*/
		{
			
			xSemaphoreGive( xSemaphore );
			buttonState=released;
		}
		
		vTaskDelay(20);  /* periodicity : 20 ms */
	}
}


void LED_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) 5 ) == pdTRUE )
		{
			LEDState=(pinState_t)(LEDState^0x01);
			GPIO_write(PORT_0,PIN1,LEDState);
			
		}
	}
}

/**********************************************************************************************************************/
