#include <string.h>
#include "TM4C123GH6PM.h"
#include <stdint.h>
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

#define LCD_DATA_DATA GPIO_PORTB_DATA_R
#define LCD_CTRL_DATA GPIO_PORTA_DATA_R
#define RS (1U<<6) 
#define EN (1U<<7) 

//Functions to initialize the LCD
void lcdCmd(unsigned char command);
void lcdStart(void);
void lcdData(unsigned char data);
//Function to clear the LCD
void lcdClear (void);
//Move the cursor to the first or second line
void lcdLine(uint8_t line);
//Display characters on the LCD
void lcdDisplay(char* name);

