/*-----------------------------------------------------------------------------------------------------------------------------------
- Software Name: Car
- Version: 1.0
- Language: C
- Developer: Emilio Rivera Macías
- Date: 01/12/2022
- Contact: emilioriveramacias@gmail.com
-----------------------------------------------------------------------------------------------------------------------------------*/
// CPU Frequency
#define F_CPU 1000000UL
// necessary includes ---------------------------->
#include <xc.h>
#include <util/delay.h>
#include <avr/io.h>

// macro declaration ---------------------------->
#define BYTE unsigned char
#define LEFT OCR0A
#define RIGHT OCR1A
#define FOWARD 0b00010100
#define BACKWARD 0b00101000
#define RIGHT_DIRECTION 0b00100100
#define LEFT_DIRECTION 0b00011000
#define STOP 0x00

// functions declaration ---------------------------->
void system_init();
void sendbyte(BYTE);
void sendstr(BYTE *);
BYTE receivebyte();

// global variables ---------------------------->
// response messages to user input
BYTE foward_message[] = "Foward\n";
BYTE right_message[] = "Right\n";
BYTE left_message[] = "Left\n";
BYTE backwards_message[] = "Backwards\n";
BYTE stop_message[] = "Stop\n";
// variable that will store user input
BYTE message = ' ';

// functions implementation ---------------------------->
/*-----------------------------------------------------------------------------------------------------------------------------------
- Functionality: initialize the necessary registers for the program to work
-----------------------------------------------------------------------------------------------------------------------------------*/
void system_init(){
    // bluetooth configuration
    // baud rate register value initialization
    UBRR0 = 51;
    // enable transmission (TXD pin) and reception (RXD pin)
    UCSR0B |= (1<<TXEN0) | (1<<RXEN0);		
    // no parity, asynchronous mode, 1 stop bit and 1 byte size
    UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
    
    //PWM configuration
    // 8 bit PWM ----->
    // clear OC0A on compare match, fast PWM and update OCR0A at bottom
    TCCR0A = 1 << WGM00 | 1 << WGM01 | 1 << COM0A1;
    // clk/8 prescalar
    TCCR0B = 1 << CS01;
    // 16 bit PWM ----->
    // clear OC1A on compare match, fast PWM and update OCR0A at bottom
    TCCR1A = 1 << WGM10 | 1 << WGM11 | 1 << COM1A1;
    // clk/8 prescalar
    TCCR1B = 1 << CS11;
    // I/O configuration
    // motors direction and OC01
    DDRD = 0b01111100;
    // OC1A
    DDRB = 0b00000010;
    // working led (to make test if everything is working correctly)
    DDRC = 0b00100000;
    // start without moving the motors
    PORTD = STOP;
}

/*-----------------------------------------------------------------------------------------------------------------------------------
- Functionality: send a byte of information using the RXD
-----------------------------------------------------------------------------------------------------------------------------------*/
void sendbyte(BYTE MSG){
    // wait if a byte is previously being transmitted
    while((UCSR0A&(1<<UDRE0)) == 0);
    // send the parameter byte throught the RXD
    UDR0 = MSG; 
}

/*-----------------------------------------------------------------------------------------------------------------------------------
- Functionality: send a string using the RXD
-----------------------------------------------------------------------------------------------------------------------------------*/
void sendstr(BYTE *string){
	unsigned char i = 0;
	// send 1 byte at a time
    while(s[i] != '\0'){
        sendbyte(string[i]);
        i++;
	}
}

/*-----------------------------------------------------------------------------------------------------------------------------------
- Functionality: receive a byte through the TXD
-----------------------------------------------------------------------------------------------------------------------------------*/
BYTE receivebyte(){
	// wait for the TXD to receive the whole packet
    while(!(UCSR0A & (1<<RXC0)));
	// return the received byte
    return UDR0;
}

// main function ---------------------------->
int main(void) {
    // initialize the system
    system_init();
    // procedure
    while(1){
        // receive a byte from TXD
        message = receivebyte();
        // switch to set the movement of the car
        switch(message){
            case 'b':
                LEFT = 219;
                RIGHT = 0xffff; 
                PORTD = FOWARD;
                PORTC = 0b00100000;
                sendstr(foward_message);   
                break;
            case 'l':
                LEFT = 219;
                RIGHT = 0;
                PORTD = LEFT_DIRECTION;
                PORTC = 0b00100000;
                sendstr(right_message);
                break;
            case 'r':
                LEFT = 0;
                RIGHT = 0xffff;
                PORTD = RIGHT_DIRECTION;
                PORTC = 0b00100000;
                sendstr(left_message);
                break;
            case 'f':
                LEFT = 219;
                RIGHT = 0xffff;
                PORTD = BACKWARD;
                PORTC = 0b00100000;
                sendstr(backwards_message);
                break;
            case 's':
                LEFT = STOP;
                RIGHT = STOP;
                PORTD = STOP;
                PORTC = 0x00;
                sendstr(stop_message);
                break;
        }
    }
    return 0;
}