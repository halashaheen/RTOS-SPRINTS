
 /*************************************************** Task 3 *************************************************************/

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h" 
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

void Button1_TriggerTask(void * pvParameters);
void Button2_TriggerTask(void * pvParameters);
void SendStr100msTask(void * pvParameters);
void UART_Write(void * pvParameters);

/*-----------------------------------------------------------*/

TaskHandle_t Button1_TriggerTaskHandler =NULL;
TaskHandle_t Button2_TriggerTaskHandler =NULL;
TaskHandle_t SendStr100msTaskHandler =NULL;
TaskHandle_t UART_WriteTaskHandler =NULL;


#define Button1_TriggerTaskStack 100
#define Button2_TriggerTaskStack 100
#define SendStr100msTaskStack 100
#define UART_WriteTaskStack 100


QueueHandle_t xQueue; 

typedef enum{
	RisingEdge,
  FallingEdge,
	Level_Low,
	Level_High
}ButtonStatus;

typedef struct{
	uint8_t* string;
}message;


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
	
  xQueue = xQueueCreate(3, sizeof(message) );
	xTaskCreate(Button1_TriggerTask,"Button1 Trigger Task",Button1_TriggerTaskStack,NULL,2,&Button1_TriggerTaskHandler);
	xTaskCreate(Button2_TriggerTask,"Button2 Trigger Task",Button2_TriggerTaskStack,NULL,2,&Button2_TriggerTaskHandler);
	xTaskCreate(SendStr100msTask,"Send String 100ms Task",SendStr100msTaskStack,NULL,1,&SendStr100msTaskHandler);
	xTaskCreate(UART_Write,"UART Write Task",UART_WriteTaskStack,NULL,1,&UART_WriteTaskHandler);


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

/*
"B1_RisingEdge \n"
"B1_FallingEdge\n"
"Button1 NoEdge\n"
*/

ButtonStatus B1_status=Level_High;
ButtonStatus B2_status=Level_High;

message button1_message={(uint8_t *)"---BUTTON 1---\n"};
message button2_message={(uint8_t *)"---BUTTON 2---\n"};
message SendStr_message={(uint8_t *)"Hello World !!\n"};

message BufferdMessage;

uint8_t B1,B2;

/*-----------------------------------------------------------*/
void Button1_TriggerTask(void * pvParameters)
{
	for( ; ; ) 
	{
		B1=GPIO_read(PORT_0,PIN0);
		
		if(B1==0)
		{
			if(B1_status==Level_High)
			{
				/*falling edge */
				B1_status=FallingEdge;
				button1_message.string=(uint8_t *)"B1_FallingEdge\n";
			}
			else
			{
				/*Low Level*/
				B1_status=Level_Low;
				button1_message.string=(uint8_t *)"Button1 NoEdge\n";
				
			}
		}
		else /*B1==1*/
		{
			if(B1_status==Level_Low)
			{
				/*rising edge*/
				B1_status=RisingEdge;
				button1_message.string=(uint8_t *)"B1_RisingEdge \n";
			}
			else
			{
				/*High Level*/
				B1_status=Level_High;
				button1_message.string=(uint8_t *)"Button1 NoEdge\n";
			}
			
		}
		
		if(B1_status<2)
			{if( xQueue != 0 )xQueueSend( xQueue,( void * ) &button1_message,( TickType_t ) 10 );}
     
		vTaskDelay(20);  /*periodicity : 20 ms */
	}
}

void Button2_TriggerTask(void * pvParameters)
{
	for( ; ; ) 
	{
		B2=GPIO_read(PORT_0,PIN1);
		if(B2==0)
		{
			if(B2_status==Level_High)
			{
				/*falling edge */
				B2_status=FallingEdge;
				button2_message.string=(uint8_t *)"B2_FallingEdge\n";
			}
			else
			{
				/*Low Level*/
				B2_status=Level_Low;
				button2_message.string=(uint8_t *)"Button2 NoEdge\n";
				
			}
		}
		else /*B2==1*/
		{
			if(B2_status==Level_Low)
			{
				/*rising edge*/
				B2_status=RisingEdge;
				button2_message.string=(uint8_t *)"B2_RisingEdge \n";
			}
			else
			{
				/*High Level*/
				B2_status=Level_High;
				button2_message.string=(uint8_t *)"Button2 NoEdge\n";
			}
			
		}
		
		if(B2_status<2)
			{if( xQueue != 0 )xQueueSend( xQueue,( void * ) &button2_message,( TickType_t ) 10 );}
		
	
		vTaskDelay(30);  /*periodicity : 30 ms */
	}
}



void SendStr100msTask(void * pvParameters)
{
	for( ; ; ) 
	{
		if( xQueue != 0 ){ xQueueSend( xQueue,( void * ) &SendStr_message,( TickType_t ) 10 );}
		
		vTaskDelay(100);  /* periodicity : 100 ms */
	}
		
		
}

void UART_Write(void * pvParameters)
{
	for( ; ; ) 
	{
		 if( xQueueReceive( xQueue,&(BufferdMessage ),( TickType_t ) 10 ) == pdPASS )
      {
         /* BufferdMessage now contains a copy of xMessage. */
				vSerialPutString((const signed char *)(BufferdMessage.string),15);
      }
		
		
		vTaskDelay(40);  /* periodicity : 100 ms */
	}
}


/**********************************************************************************************************************/
