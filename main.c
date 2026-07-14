#include "BIT_MATH.h"
#include "LCD.h"
#include "KEYBAD.h"
#include <avr/delay.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <stdio.h>

uint16_t EEMEM eeprom_total_mins; 

void add_to_lifetime_stats(int mins) {
    eeprom_busy_wait(); 
    uint16_t current_total = eeprom_read_word((uint16_t*)0);
    
    if (current_total == 0xFFFF) current_total = 0;
    
    current_total += mins;
    
    eeprom_busy_wait();
    eeprom_update_word((uint16_t*)0, current_total);
}

int countdown(int minutes, char* label) {
    long total_seconds = (long)minutes * 60;

    while (total_seconds >= 0) {
        LCD_CMD(0x01);
        LCD_STR(label);
        LCD_CMD(0xC0);

        int m = total_seconds / 60;
        int s = total_seconds % 60;

        LCD_INT(m);
        LCD_STR(":");
        if (s < 10) LCD_CHAR('0');
        LCD_INT(s);

        for (int i = 0; i < 100; i++) {
            unsigned char check = KEYPAD_GET_CHAR_NON_BLOCKING();
            if (check == 'C') return 1;
            _delay_ms(1);
        }
        total_seconds--;
    }
    return 0;
}



int get_number_input(char* prompt) {
    LCD_CMD(0x01);
    LCD_STR(prompt);
    LCD_CMD(0xC0);
    
    int value = 0;
    int digits = 0;
    while(1) {
        unsigned char key = KEYPAD_GET_CHAR();
        if (key == 'C') return -1;
        if (key >= '0' && key <= '9' && digits < 3) {
            LCD_CHAR(key);
            value = (value * 10) + (key - '0');
            digits++;
        }
        if (key == 'x') return value;
    }
}

int main(void) {
    LCD_init();
    KEYPAD_Init();
    DDRA = 0XFF;
    DDRB = 0XFF;
    DDRC = 0XFF;

    start_over: 
    LCD_CMD(0x01);
    LCD_STR("/:Start =:State");
    LCD_CMD(0xC0);
    LCD_STR("-:Reset C:Clear");

    unsigned char mode_key;
    do {
        mode_key = KEYPAD_GET_CHAR();
        if (mode_key == '=') {
            eeprom_busy_wait();
            uint16_t total = eeprom_read_word((uint16_t*)0);

            LCD_CMD(0x01);
            LCD_STR("Lifetime Work:");
            LCD_CMD(0xC0);
            LCD_INT((int)total);
            LCD_STR(" Mins");
            _delay_ms(3000);
            goto start_over;
        }
        else if(mode_key == '-') {
            eeprom_busy_wait();
            eeprom_update_word((uint16_t*)0, 0);
            eeprom_busy_wait();
            
            LCD_CMD(0X01);
            LCD_STR("Lifetime Cleared!");
            _delay_ms(2000);

            goto start_over;
        }
    } while(mode_key != '/');


    int work_mins = get_number_input("Work Mins:");
    if (work_mins == -1) goto start_over;

    int rest_mins = get_number_input("Rest Mins:");
    if (rest_mins == -1) goto start_over;

    for (int cycle = 1; cycle <= 4; cycle++) {
        char work_label[20];
        sprintf(work_label, "Cycle %d: WORK", cycle);
        if (countdown(work_mins, work_label)) {
            goto start_over;
        }

        SIT_BIT(PORTC, PC6);
       
       
        SIT_BIT(PORTB, PB7);
        _delay_ms(500);
        CLEAR_BIT(PORTB, PB7);
        _delay_ms(500);  
        SIT_BIT(PORTB, PB7);
        _delay_ms(500);
        CLEAR_BIT(PORTB, PB7);
        _delay_ms(500);       
        SIT_BIT(PORTB, PB7);
        _delay_ms(500);
        CLEAR_BIT(PORTB, PB7);
    


        
        CLEAR_BIT(PORTC, PC6);

        add_to_lifetime_stats(work_mins);

        if (cycle < 4) {
            char rest_label[20];
            sprintf(rest_label, "Cycle %d: REST", cycle);
            if (countdown(rest_mins, rest_label)) goto start_over;
        }


        SIT_BIT(PORTA, PA4);
        _delay_ms(500);
        CLEAR_BIT(PORTA, PA4);
        _delay_ms(500);  
        SIT_BIT(PORTA, PA4);
        _delay_ms(500);
        CLEAR_BIT(PORTA, PA4);
        _delay_ms(500);       
        SIT_BIT(PORTA, PA4);
        _delay_ms(500);
        CLEAR_BIT(PORTA, PA4);

    }

    LCD_CMD(0x01);
    LCD_STR("Session Finished!");
    _delay_ms(3000);
    goto start_over;
}