/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */


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
void LED_Task(void * pvParameters);


/*-----------------------------------------------------------*/

TaskHandle_t ButtonTaskHandler =NULL;
TaskHandle_t LED_TaskHandler =NULL;

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

pinState_t buttonState;
pinState_t LEDState;

uint8_t counter =0;
uint8_t pressPeriod=0;

typedef enum{
	LED_OFF,
	LED_Toggle100ms,
	LED_Toggle400ms
}LED_State;

typedef enum{
	initial_state,
	pressed,
	released
}buttonStates;

buttonStates buttonClickState=initial_state;
LED_State currLEDState;

/*-----------------------------------------------------------*/

void Button_Task(void * pvParameters)
{
	for( ; ; ) 
	{
		/* assume gpio pin to be pull-up which means 
		 * when the button is pressed it reads 0
		 * when the button is released/floatng it reads 1
		*/
		buttonState= GPIO_read(PORT_0,PIN0);
		
		if(buttonState ==PIN_IS_LOW && buttonClickState !=pressed ) /*pressed */
		{
			pressPeriod=0;
			buttonClickState=pressed;
			
		}
		else if(buttonState ==PIN_IS_HIGH) /*released */
		{
			if(buttonClickState==pressed)
			  {
					buttonClickState=released;
				}
		}
		if(buttonClickState==pressed) pressPeriod++; 
		
		vTaskDelay(40);  /* periodicity : 40 ms */
	}
}





void LED_Task(void * pvParameters)
{
	for( ; ; ) 
	{
			if((buttonClickState==released && pressPeriod<0x32) || (buttonClickState==pressed && currLEDState==LED_OFF)) /*periodicity 50ms*/
			{
				if(currLEDState!=LED_OFF) counter=0;
				counter++;
				currLEDState=LED_OFF;
				if(counter==1)
				{
					counter=0;
					GPIO_write(PORT_0,PIN1,PIN_IS_LOW);
					LEDState=PIN_IS_LOW;
				}
				
			}
			else if((buttonClickState==released && pressPeriod>0x64) || (buttonClickState==pressed && currLEDState==LED_Toggle100ms)) /*periodicity 100ms */
			{
				if(currLEDState!=LED_Toggle100ms) counter=0;
				counter++;
				currLEDState=LED_Toggle100ms;
				if(counter==1)
				{
					LEDState=(pinState_t)(LEDState^0x01);
					GPIO_write(PORT_0,PIN1,LEDState);
				}
				else if (counter==2)
				{
					counter=0;
				}
			}
			else if((buttonClickState==released && pressPeriod>0x32 && pressPeriod<0x64) || (buttonClickState==pressed && currLEDState==LED_Toggle400ms))/*periodicity 400ms */
			{
				if(currLEDState!=LED_Toggle400ms) counter=0;
				counter++;
				currLEDState=LED_Toggle400ms;
				if(counter==1)
				{
					LEDState=(pinState_t)(LEDState^0x01);
					GPIO_write(PORT_0,PIN1,LEDState);
				}
				else if (counter==8)
				{
					counter=0;
				}
			}
				
		vTaskDelay(50);
	}
	
	
}



/**********************************************************************************************************************/