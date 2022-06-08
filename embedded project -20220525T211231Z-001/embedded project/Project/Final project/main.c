#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#include "StackMacros.h"
#include "LCD.h"
#include "Uart.h"


QueueHandle_t xUARTQueue;          // UART Queue handle
QueueHandle_t xLCDQueue;           //LCD Queue handle
QueueHandle_t xBuzzerQueue;        // Buzzer Queue handle




//   Global Variables
volatile unsigned char temperature=0;


//   Function Prototypes
void Ledandbuzzer_init(void);
void Tempsensor_init(void);



static void vTask1( void *pvParameters );
static void vTask2( void *pvParameters );
static void vTask3( void *pvParameters );
static void vTask4( void *pvParameters );

// main function

int main(void){
	
xUARTQueue = xQueueCreate(1, 1);      // Create UART queue
xLCDQueue = xQueueCreate(1,8);        // Create LCD queue
xBuzzerQueue = xQueueCreate(1, 1);    // Create Buzzer queue
       
// Tasks Creation
	
	xTaskCreate( vTask1, (const portCHAR *)"Task1", configMINIMAL_STACK_SIZE, NULL, 10, NULL );
	xTaskCreate( vTask2, (const portCHAR *)"Task2", configMINIMAL_STACK_SIZE, NULL, 10, NULL );
	xTaskCreate( vTask3, (const portCHAR *)"Task3", configMINIMAL_STACK_SIZE, NULL, 10, NULL );
  xTaskCreate( vTask4, (const portCHAR *)"Task4", configMINIMAL_STACK_SIZE, NULL, 10, NULL );
	
	vTaskStartScheduler();    /* Start the scheduler. */
}



/*------------------------Main controller Task--------------------------------*/

static void vTask1( void *pvParameters )
{
unsigned const char AlarmValue = 50;      // inital Alarm value
unsigned char setpoint=30;                // inital setpoint	
char on =1;
char off =0;
typedef struct Message
{
char Txt1;
char Txt2;
} AMessage;
AMessage msg;
Ledandbuzzer_init();            //config Led & buzzer as output
Tempsensor_init();              //LM35 temp sensor initialization
while(1)
	{
		xQueueReceive( xUARTQueue, &setpoint, 0);
		 while((ADC1_RIS_R & 8) == 0){}                    /* wait for conversion complete */
     temperature = 147 - (247 * ADC1_SSFIFO3_R) / 4096;
     ADC1_ISC_R = 8;          /* clear completion flag */
		
		if(temperature<setpoint){
          GPIO_PORTC_DATA_R |= 0x10;  //Led and heater ON
        }
        else{
            GPIO_PORTC_DATA_R = 0x00;  //Led and heater OFF
					  
        }
    msg.Txt1 = temperature;
    msg.Txt2 = setpoint;
    xQueueSend (xLCDQueue, &msg, 0);
				
		if(temperature > AlarmValue)
			{
     xQueueSend(xBuzzerQueue,&on, 0);
		  }
    else xQueueSend(xBuzzerQueue,&off, 0);		
}
}
/*--------------------------Uart Controller Task-------------------------*/
static void vTask2( void *pvParameters )
{	
unsigned char N;
unsigned char Total;
UART0_Init();	
while(1)
{
 printString("Enter Temperature Setpoint(Degrees):");
 N = 0;
 Total = 0;
 while(1)
 {
   N = readChar();  // Read a number
   printChar(N);   // Echo the number
   if(N=='\r') break;
   N = N -'0';
   Total = 10*Total + N; // Total number
 }
 xQueueSend (xUARTQueue, &Total, pdMS_TO_TICKS(10)); // Send via Queue
 printString("\n\rTemperature setpoint changed...");
}
}
/*----------------------LCD Controller Task---------------------------------*/

static void vTask3( void *pvParameters )
{

	typedef struct Message
{
char Txt1;
char Txt2;
} AMessage;
AMessage msg;
LCD_init();

	for( ;; )
	{
    xQueueReceive(xLCDQueue, &msg, 0xffffffff);
    LCD_String ("Measured:");        // printing the measured value and setpoint on LCD
    LCD_Data(msg.Txt1);
    LCD_String ("Setpoint:");
		LCD_Data(msg.Txt2);
    vTaskDelay(pdMS_TO_TICKS (1000));

	}
}

/*----------------------Buzzer Controller Task---------------------------------*/

static void vTask4( void *pvParameters )
{

unsigned char Buzzerstate;  
Buzzerstate = 0;


while (1)
{
xQueueReceive(xBuzzerQueue, &Buzzerstate, 0);

if(Buzzerstate == 1)    //To check the received value from main task
	{
    GPIO_PORTC_DATA_R |= 0x20;   //Buzzer on
  }
else
	{
    GPIO_PORTC_DATA_R &= 0xDF;   //Buzzer off
  }
}
}

//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}



void Ledandbuzzer_init(void){     //LED  -> C4   Buzzer -> C5
	SYSCTL_RCGCGPIO_R  |=0x04;
	GPIO_PORTC_DIR_R = 0x30;
	GPIO_PORTC_DEN_R = 0x30;
	GPIO_PORTC_DATA_R = 0x00;

}


void Tempsensor_init(void){
    
	/* enable clocks */
	  SYSCTL_RCGCADC_R |= 2;       /* enable clock to ADC0 */
    SYSCTL_RCGCWTIMER_R |= 1;    /* enable clock to WTimer Block 0 */
 
    /* initialize ADC0 */
    ADC1_ACTSS_R &= ~8;          /* disable SS3 during configuration */
    ADC1_EMUX_R &= ~0xF000;
    ADC1_EMUX_R |= 0x5000;       /* timer trigger conversion seq 0 */
    ADC1_SSMUX3_R = 0;           /* get input from channel 0 */
    ADC1_SSCTL3_R |= 0x0E;       /* take chip temperature, set flag at 1st sample */
    ADC1_ACTSS_R |= 8;           /* enable ADC0 sequencer 3 */
 
    /* initialize wtimer 0 to trigger ADC at 1 sample/sec */
    WTIMER0_CTL_R = 0;           /* disable WTimer before initialization */
    WTIMER0_CFG_R = 0x04;        /* 32-bit option */
    WTIMER0_TAMR_R = 0x02;       /* periodic mode and down-counter */
    WTIMER0_TAILR_R = 16000000;  /* WTimer A interval load value reg (1 s) */
    WTIMER0_CTL_R |= 0x20;       /* timer triggers ADC */
    WTIMER0_CTL_R |= 0x01;       /* enable WTimer A after initialization */
          

}
