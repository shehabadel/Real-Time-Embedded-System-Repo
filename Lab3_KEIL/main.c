#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

static void vTask1( void *pvParameters );


int main()
{
    __asm("CPSID I");
    SYSCTL_RCGCGPIO_R=0x20;
    GPIO_PORTF_DIR_R=0x0E;
    GPIO_PORTF_DEN_R=0x0E;
		__asm("CPSIE I");
	
		xTaskCreate(vTask1, (const portCHAR *)"Task1", 128, NULL, 1, NULL );
	
		vTaskStartScheduler();

}	

static void vTask1( void *pvParameters )
{

	for ( ; ; )
	{
		const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
		GPIO_PORTF_DATA_R ^=(1U << 1);
		vTaskDelay(xDelay);
		
		
	}	
	
}	