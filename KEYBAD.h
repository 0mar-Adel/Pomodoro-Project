#include <avr/io.h>
#include <util/delay.h>

unsigned char keypad_map[4][4] = {
    {'7', '8', '9', '/'},
    {'4', '5', '6', 'x'},
    {'1', '2', '3', '-'},
    {'C', '0', '=', '+'}
};

void KEYPAD_Init() {
    DDRC |= (0b00111100); 
    DDRD &= ~((1<<PD3) | (1<<PD5) | (1<<PD6) | (1<<PD7));
    PORTD |= ((1<<PD3) | (1<<PD5) | (1<<PD6) | (1<<PD7));
}

unsigned char KEYPAD_GET_CHAR() {
    unsigned char rows[] = {PC5, PC4, PC3, PC2};
    unsigned char cols[] = {PD7, PD6, PD5, PD3};

    for (int r = 0; r < 4; r++) {
        PORTC |= 0b00111100;
        PORTC &= ~(1 << rows[r]);

        _delay_us(10);

        for (int c = 0; c < 4; c++) {
            if (!(PIND & (1 << cols[c]))) {
                _delay_ms(30); 
                while (!(PIND & (1 << cols[c])));
                return keypad_map[r][c];
            }
        }
    }
    return 0;
}

unsigned char KEYPAD_GET_CHAR_NON_BLOCKING() {
    unsigned char rows[] = {PC5, PC4, PC3, PC2};
    unsigned char cols[] = {PD7, PD6, PD5, PD3};

    for (int r = 0; r < 4; r++) {
        PORTC |= 0b00111100;
        PORTC &= ~(1 << rows[r]);
        _delay_us(10); 

        for (int c = 0; c < 4; c++) {
            if (!(PIND & (1 << cols[c]))) {
                return keypad_map[r][c];
            }
        }
    }
    return 0;
}