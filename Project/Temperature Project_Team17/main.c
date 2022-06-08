#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include <stdint.h>
#include <stdbool.h>
#include "TM4C123GH6PM.h"
#include "inc\hw_timer.h"
#include "inc\hw_gpio.h"
#include "driverlib\timer.h"
#include "driverlib\gpio.h"
#include "driverlib\sysctl.h"
#include "inc\tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h"
#include "driverlib/interrupt.h"
#include "inc/hw_ints.h"
#include "tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "inc/hw_types.h"
#include "inc/hw_timer.h"
#include "driverlib/timer.h"
#include "driverlib/sysctl.h"
#include "driverlib/GPIO.h"
#include "lcdConfig.h"
#include "adcConfig.h"

QueueHandle_t xUARTQueue;
QueueHandle_t xLCDQueue;
QueueHandle_t xBuzzerQueue;

// function to convert to string
void toString(char tim, char text[])
{
	// initialize text [0,0]
	for (int j = 0; j < 2; j++)
	{
		text[j] = '0';
	}
	// put numbers in char array
	int i = 2;
	while (tim != 0)
	{
		i--;
		text[i] = ((tim % 10) + '0');
		tim /= 10;
	}
	text[2] = '\0'; // add null terminator
}

// function to print on the terminal through uart
void printchar(char c)
{
	while ((UART0_FR_R & (1 << 5)) != 0)
		;
	UART0_DR_R = c;
}

//Call the Auxilitary function printChar
void print(char *string)
{
	while (*string)
	{
		printchar(*(string++));
	}
}

// GPIO Init for used ports which are PORTF, PORTD, PORTE, PORTA, PORTB
// Which are used in the UART0, LEDs, Buzzer, the 2 Potentiometers
void gpiosInit()
{
	SYSCTL_RCGCGPIO_R |= 0x00000020; // Initialize clock to PORTF
	while ((SYSCTL_PRGPIO_R & 0x00000020) == 0)
	{
	} // safety for clock initialization
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R = 0x1F;	 // Enable change to PORTF
	GPIO_PORTF_DIR_R = 0x0E; // Make led ports as output
	GPIO_PORTF_DEN_R = 0x1F; // digital enable to pins
	GPIO_PORTF_PUR_R = 0x11;

	SYSCTL_RCGCGPIO_R |= 0x00000008; // Initialize clock to PORTD
	while ((SYSCTL_PRGPIO_R & 0x00000008) == 0)
	{
	}; // safety for clock initialization
	GPIO_PORTD_LOCK_R = 0x4C4F434B;
	GPIO_PORTD_CR_R = 0xFF;	 // Enable change to PORTD
	GPIO_PORTD_DIR_R = 0xFF; // Make led ports as output
	GPIO_PORTD_DEN_R = 0xFF; // digital enable to pins

	SYSCTL_RCGCGPIO_R |= 0x00000010; // Initialize clock to PORTE
	while ((SYSCTL_PRGPIO_R & 0x00000010) == 0)
	{
	}; // safety for clock initialization
	GPIO_PORTE_LOCK_R = 0x4C4F434B;
	GPIO_PORTE_CR_R = 0x2F;	 // Enable change to PORTE
	GPIO_PORTE_DIR_R = 0x2F; // Make led ports as output
	GPIO_PORTE_DEN_R = 0x2F; // digital enable to pins

	SYSCTL_RCGCUART_R |= 0X0001;	 // Activate UART0
	SYSCTL_RCGCGPIO_R |= 0x00000001; // Activate PortA
	UART0_CTL_R &= ~0x0001;			 // Disable UART0 Before initilization
	UART0_CC_R = 0X0;
	UART0_IBRD_R = 104;			// Integer Baud
	UART0_FBRD_R = 11;			// Fractional Baud
	UART0_LCRH_R = (0x3 << 5);	// 8-bit length enable fifo
	GPIO_PORTA_AFSEL_R |= 0X03; // Alt on PA/0
	GPIO_PORTA_PCTL_R = 0X011;
	GPIO_PORTA_DEN_R |= 0X03; // Enable digital I/O
	UART0_CTL_R = 0x0301;	  // Enable UART0 after initilization

	lcdStart();
}

// main controller task
void TASK1(void *pvParameters)
{

	typedef struct Message
	{
		char Txt1[4];
		char Txt2[4];
	} AMessage;
	AMessage msg;

	char *on;
	char *off;
	on = 1;
	off = 0;
	unsigned char setpoint = 30; //Setpoint default temperature
	unsigned AdcValue; //Value read from the ADC
	unsigned char Temperature; //Temperature that is read from the potentiometer
	float mV; //Value of ADC after being converted to Celcius
	unsigned const char AlarmValue = 50;
	adc_init(); //ADC Initilization
	while (1)
	{
		xQueueReceive(xUARTQueue, &setpoint, 0); // Receive data
		AdcValue = adc_read();					 // Read ADC
		mV = 147 - (247 * AdcValue) / 4096;		 // Temp in C
		Temperature = (int)mV;					 // Temp as integer
		if (Temperature < setpoint)
		{							   // If cold
			GPIO_PORTE_DATA_R |= 0x02; // Heater LED ON
		}
		else // If hot
		{
			GPIO_PORTE_DATA_R &= ~0x02; // Heater Led OFF
		}
		toString(Temperature, msg.Txt1);	  // Measured value
		toString(setpoint, msg.Txt2);		  // setpoint
		xQueueSend(xLCDQueue, &msg, 0);		  // Send via Queue
		if (Temperature > AlarmValue)		  // Alarm?
			xQueueSend(xBuzzerQueue, &on, 0); // buzzer ON
		else
			xQueueSend(xBuzzerQueue, &off, 0); // Buzzer off
	}
}

// uart controller
void TASK2(void *pvParameters)
{
	unsigned N;
	unsigned AdcValue;
	unsigned char Total;
	while (1)
	{
		print("\n\r\nEnter Temperature Setpoint (Degrees): ");
		N = 0;
		Total = 0;
		while (1)
		{
			while ((UART0_FR_R & (1 << 4)) != 0)
				; // read from the user the setpoint number
			N = UART0_DR_R; //Read a number
			print(&N); //Print the number
			if (N == '\r') //If enter was pressed
				break;
			N = N - '0'; //Pure Number converted from char
			Total = 10 * Total + N; //Total Number
		}
		xQueueSend(xUARTQueue, &Total, pdMS_TO_TICKS(10)); //Send via Queue
		print("\n\rTemperature Setpoint changed...");
	}
}

// lcd controller
void TASK3(void *pvParameters)
{
	typedef struct Message
	{
		char Txt1[4];
		char Txt2[4];

	} AMessage;
	AMessage msg;
	lcdClear(); //Clear LCD
	while (1)
	{
		xQueueReceive(xLCDQueue, &msg, 0); //Receive Data
		lcdLine(1); 
		lcdDisplay("Measure: "); //Heading
		lcdDisplay(msg.Txt1); //Display temperature
		lcdLine(2);
		lcdDisplay("Setpoint: "); //Heading
		lcdDisplay(msg.Txt2); //Display Setpoint
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

// buzzer controller
void TASK4(void *pvParameters)
{
	unsigned char Buzzerstate;
	Buzzerstate = 0;  //Default State
	while (1)
	{
		xQueueReceive(xBuzzerQueue, &Buzzerstate, 0); //Get data from the queue
		if (Buzzerstate == 1) //Alarm
		{
			GPIO_PORTE_DATA_R |= 0x05; // Buzzer & Buzzer Led ON
		}
		else
		{
			GPIO_PORTE_DATA_R &= ~0x05; // Buzzer & Buzzer Led OFF
		}
	}
}

int main(void)
{
	//Create Queues for UART, LCD, and Buzzer
	xUARTQueue = xQueueCreate(1, 1);
	xLCDQueue = xQueueCreate(1, 8);
	xBuzzerQueue = xQueueCreate(1, 1);

	//GPIO ports Init
	gpiosInit();
	
	//Task Creations
	xTaskCreate(TASK1, "MAIN_Controller", 100, NULL, 1, 0);
	xTaskCreate(TASK2, "UART_Controller", 100, NULL, 1, 0);
	xTaskCreate(TASK3, "LCD_Controller", 100, NULL, 1, 0);
	xTaskCreate(TASK4, "Buzzer_Controller", 100, NULL, 1, 0);
	//Start the OS Kernel
	vTaskStartScheduler();
}
