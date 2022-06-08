#include "LCD.h"



/* LCD and GPIOB initialization Function */ 
void LCD_init(void)
{
  /* Enable Clock to GPIOB & GPIOA */
 SYSCTL_RCGCGPIO_R  |=0x02;
 GPIO_PORTB_AFSEL_R &= ~0xff;    //disable alternative functions for portB
 GPIO_PORTB_AMSEL_R &= ~0Xff;    //disable analogue function
 GPIO_PORTB_PCTL_R &= ~0XFF;  
 GPIO_PORTB_DEN_R   =0xFF;        /* Declare GPIOB pins as digital pins */   
 GPIO_PORTB_DIR_R   =0xFF;        /* Set GPIOB all pins a digital output pins */          
            
 
 SYSCTL_RCGCGPIO_R  |=0x01;
// GPIO_PORTA_AFSEL_R &= ~0xE0;    //disable alternative functions for PA5,6,7
// GPIO_PORTA_AMSEL_R &= ~0XE0;    //disable analogue function for PA5,6,7
// GPIO_PORTA_PCTL_R &= ~0XE0;     //regular digital pins
 GPIO_PORTA_DEN_R |= 0xE0; 
 GPIO_PORTA_DIR_R |= 0xE0;           
                                  
 
LCD_Cmd(0x38); //8-bits,2 display lines, 5x7 font
LCD_Cmd(0x06); //increments automatically
LCD_Cmd(0x0F); //Turn on display
LCD_Cmd(0x01); //clear display
 
}

void LCD_Cmd(unsigned char command)
{
  
GPIO_PORTA_DATA_R = 0x00; //RS =0, E=0, RW=0
GPIO_PORTB_DATA_R =command;
GPIO_PORTA_DATA_R = 0x80; //E=1 to secure command
delay_micro(0);
GPIO_PORTA_DATA_R = 0x00;
if(command <4) 
   delay_milli(2); 
else 
   delay_micro(37);

    
}


void LCD_Data(unsigned char data)
{
  GPIO_PORTA_DATA_R = 0x20; //RS =1, E=0, RW=0
GPIO_PORTB_DATA_R =data;
GPIO_PORTA_DATA_R |= 0x80; //E=1 to secure command
GPIO_PORTA_DATA_R = 0x00;
delay_micro(0);
 
}

void LCD_String (char *str)	/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)  /* Send each char of string till the NULL */
	{
		LCD_Data(str[i]);  /* Call LCD data write */
	}
}


/* Mili seconds delay function */
void delay_milli(int n)
{
 int i,j;
 for(i=0;i<n;i++)
 for(j=0;j<3180;j++)
 {}
}

/* Micro seconds delay function */
void delay_micro(int n)
{
 int i,j;
 for(i=0;i<n;i++)
 for(j=0;j<3;j++)
 {}
 
}


