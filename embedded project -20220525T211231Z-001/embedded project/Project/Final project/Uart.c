#include "Uart.h"
#include "LCD.h"



char readChar(void)
{
	char d;
	while((UART0_FR_R & 0x010 ) != 0){ }
	
		d = UART0_DR_R;
	return (unsigned char)d;
}

void printChar(char c)
{
	while((UART0_FR_R &0x20) != 0){ }
		UART0_DR_R =c;
	
}

void printString(char *string)
{
	while(* string)
	{
		printChar(*(string++));
		delay_milli(1);
	}
}


void UART0_Init(void)
{
	SYSCTL_RCGCUART_R |= 0x01;
  SYSCTL_RCGCGPIO_R |= 0x00000001;     
  GPIO_PORTA_AFSEL_R |= 0x03;
  GPIO_PORTA_PCTL_R |= 0x00000011;   
  GPIO_PORTA_DEN_R |= 0x03;
	GPIO_PORTA_AMSEL_R |= 0x03;
	UART0_CTL_R = 0;
	// BRD=16000000/(16*9600)= 140.1666666666666666666666666666666666666666666666666666666666666666667
	//UARTFBRD= integer(0.166666667*64+0.5)=11
	UART0_IBRD_R = 104;
	UART0_FBRD_R= 11;
	UART0_LCRH_R =0x60; //8- data bit,no parity, 1 stop bit
	UART0_CC_R= 0x0;
	UART0_CTL_R=0x301;
}




