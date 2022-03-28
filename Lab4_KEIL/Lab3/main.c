#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

static void vTask1( void *pvParameters );
static void vTask2( void *pvParameters );
static void vTask3( void *pvParameters );


int main()
{
    __asm("CPSID I");
    SYSCTL_RCGCGPIO_R=0x20;
    GPIO_PORTF_DIR_R=0x0E;
    GPIO_PORTF_DEN_R=0x0E;
		__asm("CPSIE I");
	
		xTaskCreate(vTask1, (const portCHAR *)"Task1", 128, NULL, 1, NULL );
		xTaskCreate(vTask2, (const portCHAR *)"Task2", 128, NULL, 1, NULL );
		xTaskCreate(vTask3, (const portCHAR *)"Task3", 128, NULL, 1, NULL );
	
		vTaskStartScheduler();

}	

static void vTask1( void *pvParameters )
{
	
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	for ( ; ; )
	{
		//const TickType_t xDelay = 500 / portTICK_RATE_MS;
		GPIO_PORTF_DATA_R ^=(1U << 1);
		//vTaskDelay(1000/portTICK_RATE_MS);
		vTaskDelayUntil(&xLastWakeTime,(1000/portTICK_RATE_MS));
		//vTaskDelay(xDelay);
		
		
	}	
	
}	

static void vTask2( void *pvParameters )
{
	
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	for ( ; ; )
	{
		
		
		//const TickType_t xDelay = 500 / portTICK_RATE_MS;
		GPIO_PORTF_DATA_R ^=(1U << 2);
		//vTaskDelay(2000/portTICK_RATE_MS);
		vTaskDelayUntil(&xLastWakeTime,(2000/portTICK_RATE_MS));
		//vTaskDelay(xDelay);
		
		
	}	
	
}	

static void vTask3( void *pvParameters )
{
	
	portTickType xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();

	for ( ; ; )
	{
			
		//const TickType_t xDelay = 500 / portTICK_RATE_MS;
		GPIO_PORTF_DATA_R ^=(1U << 3);
		//vTaskDelay(3000/portTICK_RATE_MS);
		vTaskDelayUntil(&xLastWakeTime,(3000/portTICK_RATE_MS));
		//vTaskDelay(xDelay);
		
		
	}	
	
}	