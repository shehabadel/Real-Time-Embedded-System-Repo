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
#include "LCD.h"
#include "ADC.h"
#include "tm4c123gh6pm.h"
#include "TM4C123GH6PM.h"

// using ANO on PE3	




void adc_init (){
	
	// GIVING CLCOK
	SYSCTL_RCGCGPIO_R|=(1<<4);  // give clock to port E 
	SYSCTL_RCGCADC_R |=  1; /* AD0 clock enable*/ 
	
	
	GPIO_PORTE_AFSEL_R |= (1UL<<3);   /* enable alternate function */
  GPIO_PORTE_DEN_R &= ~(1UL<<3);    /* disable digital function */
  GPIO_PORTE_AMSEL_R |= (1UL<<3);   /* enable analog function */
	
	
	ADC0_ACTSS_R &= ~(1UL<<3);/* disable SS3 during configuration */
  ADC0_EMUX_R &= ~0xF000;    /* software trigger conversion */
	ADC0_SSMUX3_R = 0;         /* get input from channel 0 */
	ADC0_SSCTL3_R |= (1<<1)|(1<<2);        /* take one sample at a time, set flag at 1st sample */
	ADC0_ACTSS_R |= (1<<3);         /* enable ADC0 sequencer 3 */
		
}

 

unsigned int adc_read (){ 
	unsigned int val ;
	ADC0_PSSI_R |= (1UL<<3);
	while((ADC0_RIS_R & 8) == 0) {} //wait for conversion to be completed
	
	val = ADC0_SSFIFO3_R; //reads the value
	ADC0_ISC_R = 8;          /* clear coversion clear flag bit*/
	return val ;	
}