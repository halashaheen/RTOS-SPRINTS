/*************************************************** Task 3 *************************************************************/

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

void Button_Task(void * pvParameters);
void LED_OFF_Task(void * pvParameters);
void LED_Toggle100ms_Task(void * pvParameters);
void LED_Toggle400ms_Task(void * pvParameters);

/*-----------------------------------------------------------*/

TaskHandle_t ButtonTaskHandler =NULL;
TaskHandle_t LED_OFF_TaskHandler =NULL;
TaskHandle_t LED_100ms_TaskHandler =NULL;
TaskHandle_t LED_400ms_TaskHandler =NULL;

#define ButtonTaskStack 100
#define LED_OFF_TaskStack 100
#define LED_100ms_TaskStack 100
#define LED_400ms_TaskStack 100


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
	
	xTaskCreate(Button_Task,"Button Task",ButtonTaskStack,NULL,0,&ButtonTaskHandler);
	xTaskCreate(LED_OFF_Task,"LED_OFF Task",LED_OFF_TaskStack,NULL,0,&LED_OFF_TaskHandler);
    xTaskCreate(LED_Toggle100ms_Task,"LED_100ms Task",LED_100ms_TaskStack,NULL,0,&LED_100ms_TaskHandler);
	xTaskCreate(LED_Toggle400ms_Task,"LED_400ms Task",LED_400ms_TaskStack,NULL,0,&LED_400ms_TaskHandler);

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

pinState_t buttonState;
pinState_t LEDState;

TickType_t START_IN_TICKS;
TickType_t END_IN_TICKS;
TickType_t TIME_IN_TICKS;

/*-----------------------------------------------------------*/

void Button_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		buttonState= GPIO_read(PORT_0,PIN0);
		if(buttonState ==PIN_IS_LOW)
		{
			START_IN_TICKS= xTaskGetTickCount();
		}
		else 
		{
			END_IN_TICKS= xTaskGetTickCount();
			TIME_IN_TICKS= END_IN_TICKS-START_IN_TICKS;
		}
		
		vTaskDelay(20);  /* periodicity : 20 ms */
	}
}

void LED_OFF_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		
		if(!(buttonState)&&(TIME_IN_TICKS<2000))
		{
			GPIO_write(PORT_0,PIN1,PIN_IS_LOW);
			LEDState=PIN_IS_LOW;
		}

		vTaskDelay(50);	/* periodicity : 50 ms */
	}
}


void LED_Toggle100ms_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		
		if(!(buttonState)&&(TIME_IN_TICKS>4000))
		{
			LEDState=(pinState_t)(LEDState^0x01);
			GPIO_write(PORT_0,PIN1,LEDState);
		}

		vTaskDelay(100);	/* periodicity : 100 ms */
	}
}

void LED_Toggle400ms_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		if(!(buttonState)&&(TIME_IN_TICKS>2000)&&(TIME_IN_TICKS<4000))
		{
			LEDState=(pinState_t)(LEDState^0x01);
			GPIO_write(PORT_0,PIN1,LEDState);
		}

		vTaskDelay(400);	/* periodicity : 400 ms */
	}
}


/**********************************************************************************************************************/


