#include "tm4c123gh6pm.h"
#include<stdint.h>
#include<stdbool.h>


void delay_milli(int n);
void delay_micro(int n);
void LCD_init(void);
void LCD_Cmd(unsigned char command);
void LCD_Data(unsigned char data);
void LCD_String (char *str)	;