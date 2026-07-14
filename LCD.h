#include "BIT_MATH.h"
#include <avr/io.h>
#include <avr/delay.h>
#include <stdlib.h>

#define E PA2
#define RS PA3

void LCD_SEND_NIBBLE(unsigned char nibble){
    PORTB |= (1 << PB6); 

    PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4));
    if (nibble & 0x01) SIT_BIT(PORTB, 0);
    if (nibble & 0x02) SIT_BIT(PORTB, 1);
    if (nibble & 0x04) SIT_BIT(PORTB, 2);
    if (nibble & 0x08) SIT_BIT(PORTB, 4);

    SIT_BIT(PORTA, E);
    _delay_us(1);
    CLEAR_BIT(PORTA, E);
    _delay_us(100);

    PORTB &= ~(1 << PB6); 
}

void LCD_CMD(unsigned char cmd){
    CLEAR_BIT(PORTA, RS);
    LCD_SEND_NIBBLE(cmd >> 4);
    LCD_SEND_NIBBLE(cmd & 0x0F);
    _delay_ms(2);
}

void LCD_CHAR(unsigned char ch)
{
    SIT_BIT(PORTA, RS);
    LCD_SEND_NIBBLE(ch >> 4);
    LCD_SEND_NIBBLE(ch & 0x0F);
    _delay_us(100);
}

void LCD_init() {
    DDRA |= (1 << RS) | (1 << E);
    DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);

    _delay_ms(50);

    LCD_SEND_NIBBLE(0x03);
    _delay_ms(5);
    LCD_SEND_NIBBLE(0x03);
    _delay_ms(1);
    LCD_SEND_NIBBLE(0x03);
    LCD_SEND_NIBBLE(0x02);

    LCD_CMD(0X28);
    LCD_CMD(0X0C);
    LCD_CMD(0X01);
    _delay_ms(2);
}

void LCD_STR(char *str)
{
    while(*str)
    {
        LCD_CHAR(*str++);
    }
}

void LCD_INT(int num) {
    char buffer[7];
    itoa(num, buffer, 10);
    LCD_STR(buffer);
}
