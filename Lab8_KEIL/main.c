#include "tm4c123gh6pm.h"
#include <stdint.h> 
#include <FreeRTOS.h>
//#include "bsp.h"
#include "timers.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
//#include "inc/hw_memmap.h" 
//#include "inc/hw_types.h" 
//#include "inc/hw_gpio.h"
#include "TM4C123.h"

#define LED_RED   (1U << 1)

//Functions
void ms_delay(int ms);
void tiva_init(void);
void printchar(char c);
void print(char *string);

//Variables
xSemaphoreHandle xBinarySemaphore;
xSemaphoreHandle xMutex;
const TickType_t xDelay = 500 / portTICK_PERIOD_MS; // 500ms

//Tasks
void counterTask( void *pvParameters);
void ledTogglerTask( void *pvParameters);


//Implementation
void printchar(char c)
{
	while((UART0_FR_R&(1<<5))!=0);
	UART0_DR_R=c;
}

void print(char *string)
{
  while(*string)
	{
		printchar(*(string++));
  }
}

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
	GPIO_PORTF_DEN_R = 0x1FU;      
	GPIO_PORTF_PUR_R = 0X11U;
	GPIO_PORTF_DATA_R = 0X00;
	
	//Configure PORTF4 for falling edge trigger interrupt
	GPIO_PORTF_IS_R &= ~0x10; /* make bit 4, 0 edge sensitive */
	GPIO_PORTF_IBE_R &= ~0x10; /* trigger is controlled by IEV */
	GPIO_PORTF_IEV_R &= ~0x10; /* falling edge trigger */
	GPIO_PORTF_ICR_R |= 0x10; /* clear any prior interrupt */
	GPIO_PORTF_IM_R |= 0x10; /* unmask interrupt */
	NVIC_PRI7_R = 7 << 21; /* set interrupt priority to 2 */
	NVIC_EN0_R |= 1 << 30; /* enable interrupt in NVIC */
	__enable_irq(); /* global enable IRQs */
	
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

void GPIOF_Handler(void)
{	

	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
  xSemaphoreGiveFromISR( xBinarySemaphore, &xHigherPriorityTaskWoken );
  GPIO_PORTF_ICR_R |= 0x10; /* clear the interrupt flag */
  portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
	
}

void counterTask( void *pvParameters)
{
	for( ; ; )
	{
		xSemaphoreTake( xMutex, portMAX_DELAY );
		print("This is the CounterTask \n");
		for(int i = 0; i <= 10; i++)
		{
			print(i);
			print("\n");
		}
		xSemaphoreGive( xMutex );
	}
}

void ledTogglerTask( void *pvParameters)
{
	xSemaphoreTake(xBinarySemaphore, 0);
	for( ; ; )
	{
		xSemaphoreTake(xBinarySemaphore, portMAX_DELAY);
		xSemaphoreTake( xMutex, portMAX_DELAY );
		print("This is the LedToggler Task \n");
		for(int i = 0; i <= 10; i++)
		{
			print(i);
			print("\n");
		}
		GPIO_PORTF_DATA_R ^= LED_RED;
				xSemaphoreGive( xMutex );

//		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

int main( void ) 
{ 
	tiva_init();
	xMutex = xSemaphoreCreateMutex();
	vSemaphoreCreateBinary( xBinarySemaphore );
	
	if( xBinarySemaphore != NULL && xMutex != NULL )
    {
			xTaskCreate( counterTask, "Counter Task", configMINIMAL_STACK_SIZE, NULL , 1, NULL );
			xTaskCreate( ledTogglerTask, "Led Toggler Task", configMINIMAL_STACK_SIZE, NULL, 2, NULL );
			vTaskStartScheduler(); 			
		}
	for( ;; ); 
}