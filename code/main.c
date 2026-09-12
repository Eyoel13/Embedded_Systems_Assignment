#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR ((F_CPU / ( 16UL * BAUD )) - 1)


#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

ISR(INT0_vect) {
    
}


void uart_init(unsigned int ubrr) {
    UBRR0L = (uint8_t)(ubrr);
    UBRR0H = (uint8_t)(ubrr >> 8);
    UCSR0B = (1<<TXEN0);
    UCSR0C = (1<<UCSZ00) | (1<<UCSZ01);
}

void uart_transmit(char data) {
    while(!(UCSR0A & (1<<UDRE0)));
    UDR0 = data;
}

void uart_print(const char *str) {
    while(*str) {
        uart_transmit(*str++);
    }
}


void setup_hardware() {
    DDRB |= (1<<PB5) | (1<<PB2);
    PORTB &= ~((1<<PB5) | (1<<PB2));

    DDRD &= ~(1<<PD2);
    PORTD |= (1<<PD2);

    EICRA &= ~((1<<ISC01) | (1<<ISC00));
    EIMSK |= (1<<INT0);
}

void power_down_code() {
    ADCSRA &= ~(1<<ADEN);

    SMCR &= ~((1<<SM0) | (1<<SM2));
    SMCR |= (1<<SM1);

    SMCR |= (1<<SE);

    sei();

    asm volatile("sleep");

    SMCR &= ~(1<<SE);

    ADCSRA |= (1<<ADEN);
}


int main() {
    uart_init(MYUBRR);
    setup_hardware();

    while(1) {

        uart_print("Status: Seeping...\r\n");

        power_down_code();

        uart_print("Status: Awake!\r\n");

        while(!(PIND & (1<<PD2))) {
            PORTB |= (1<<PB5) | (1<<PB2);
        }

        PORTB &= ~((1<<PB5) | (1<<PB2));
    }

    return 0;
}
