#include "tm4c123gh6pm.h"
#include <stdint.h> 
#include <FreeRTOS.h>
#include "bsp.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "ARMCM4.h"

#define mainSW_INTERRUPT_ID		( ( IRQn_Type ) 0 )
#define mainTRIGGER_INTERRUPT()	NVIC_SetPendingIRQ( mainSW_INTERRUPT_ID )
#define mainCLEAR_INTERRUPT()	NVIC_ClearPendingIRQ( mainSW_INTERRUPT_ID )
#define mainSOFTWARE_INTERRUPT_PRIORITY 		( 5 )

void ms_delay(int ms);
void tiva_init(void);

xQueueHandle xQueue;
xSemaphoreHandle xBinarySemaphore;
uint32_t counter = 0;

static void Button1( void *pvParameters);
void UART_TASK( void *pvParameters);
static void prvSetupSoftwareInterrupt(void);
void vSoftwareInterruptHandler( void );

void ms_delay(int ms)
{
for (int i = 0; i < ms; i++)
	{
		for (int j = 0; j < 3180; j++)
		{}
	}
}


void tiva_init(void)
{
	//Initialize The 2 Push Buttons (PORT F)
	SYSCTL_RCGCGPIO_R = 0x20U;
	GPIO_PORTF_DIR_R = 0xEEU;
	GPIO_PORTF_DEN_R = 0X11U;
	GPIO_PORTF_PUR_R = 0X11U;
	GPIO_PORTF_DATA_R = 0X00;
	
	//Initialize UART0 (PORT A)
	SYSCTL_RCGCUART_R |= 0x01; //Enable UART0
	SYSCTL_RCGCGPIO_R |= 0x01; //Enable PORTA
	UART0_CTL_R = 0;
	UART0_IBRD_R = 104;
	UART0_FBRD_R = 11;
	UART0_CC_R = 0x0;
	UART0_LCRH_R = 0x60;
	UART0_CTL_R = 0x301;
	GPIO_PORTA_DEN_R = 0x03;
	GPIO_PORTA_AFSEL_R = 0x03;
	GPIO_PORTA_PCTL_R = 0x11;
}

static void Button1( void *pvParameters) //SW1
{
	for( ; ; )
	{
		if((GPIO_PORTF_DATA_R&0x10) == 0)
		{
			do{
				ms_delay(20);
			}
			while((GPIO_PORTF_DATA_R&0x10) == 1);
			counter++;
			xQueueSendToBack( xQueue, &counter, 0 );
			while((GPIO_PORTF_DATA_R&0x10) == 0);
			do{
				ms_delay(20);
			}
			while((GPIO_PORTF_DATA_R&0x10) == 0);
			mainTRIGGER_INTERRUPT();
		}
		taskYIELD()
	}
}

void UART_TASK( void *pvParameters)
{
	xSemaphoreTake(xBinarySemaphore, 0);
	for( ; ; )
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		uint32_t count;
		xQueueReceive( xQueue, &count, portMAX_DELAY);
		while(UART0_FR_R & 0x20); //wait until buffer not full
		UART0_DR_R = count;
	}
}

void vSoftwareInterruptHandler(void)
{
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
  mainCLEAR_INTERRUPT();
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

static void prvSetupSoftwareInterrupt( void )
{
	NVIC_SetPriority( mainSW_INTERRUPT_ID, mainSOFTWARE_INTERRUPT_PRIORITY );
	NVIC_EnableIRQ( mainSW_INTERRUPT_ID );
}

int main( void ) 
{ 
	tiva_init();
	vSemaphoreCreateBinary( xBinarySemaphore );
	xQueue = xQueueCreate( 5, sizeof( long ) );
	
	if( xBinarySemaphore != NULL )
    {
			prvSetupSoftwareInterrupt();
			xTaskCreate( Button1, "Button1", configMINIMAL_STACK_SIZE, NULL , 1, NULL );
			xTaskCreate( UART_TASK, "UART", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
			vTaskStartScheduler(); 			
		}
	for( ;; ); 
}