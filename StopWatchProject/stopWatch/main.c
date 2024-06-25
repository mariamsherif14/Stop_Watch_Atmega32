#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Functions prototype

void SevenSegmentInit(void);
void Timer1CTCInit(void);
void INT0Init(void);
void INT1Init(void);
void INT2Init(void);

void incrementSegment(void);

unsigned char interruptFlag = 0;
unsigned char seconds = 0, minutes = 0, hours = 0;

unsigned char segmentDigits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

unsigned char secondsTens = 0, secondsUnits = 0, minutesTens = 0, minutesUnits = 0, hoursTens = 0, hoursUnits = 0;
unsigned char counter = 0;

ISR(INT0_vect)
{

    PORTC = 0;
    seconds = 0;
    minutes = 0;
    hours = 0;
    secondsTens = 0;
    secondsUnits = 0;
    minutesTens = 0;
    minutesUnits = 0;
    hoursTens = 0;
    hoursUnits = 0;
}

ISR(INT1_vect)
{
    /* disable Timer1 Compare A Interrupt */
    TIMSK &= ~(1 << OCIE1A);
}

ISR(INT2_vect)
{

    /* Enable Timer1 Compare A Interrupt */
    TIMSK |= (1 << OCIE1A);
}

ISR(TIMER1_COMPA_vect)
{
    interruptFlag = 1;
}

int main(void)
{
    Timer1CTCInit();
    INT0Init();
    INT1Init();
    INT2Init();


    DDRA |= 0x3F;

    //Seven segment initialization
    SevenSegmentInit();

    SREG |= (1 << 7);
    while (1)
    {
        //set enable of the second segment from right hand side (seconds part)to display secondsTens

        PORTA &= ~(1 << PA5) & ~(1 << PA3) & ~(1 << PA2) & ~(1 << PA1) & ~(1 << PA0);
        PORTA |= (1 << PA4);
        PORTC = segmentDigits[secondsTens];
        _delay_ms(2);

        PORTA &= ~(1 << PA4) & ~(1 << PA3) & ~(1 << PA2) & ~(1 << PA1) & ~(1 << PA0);
        PORTA |= (1 << PA5);
        PORTC = segmentDigits[secondsUnits];
        _delay_ms(2);

        PORTA &= ~(1 << PA4) & ~(1 << PA3) & ~(1 << PA5) & ~(1 << PA1) & ~(1 << PA0);
        PORTA |= (1 << PA2);
        PORTC = segmentDigits[minutesTens];
        _delay_ms(2);

        PORTA &= ~(1 << PA4) & ~(1 << PA5) & ~(1 << PA2) & ~(1 << PA1) & ~(1 << PA0);
        PORTA |= (1 << PA3);
        PORTC = segmentDigits[minutesUnits];
        _delay_ms(2);

        PORTA &= ~(1 << PA4) & ~(1 << PA3) & ~(1 << PA2) & ~(1 << PA1) & ~(1 << PA5);
        PORTA |= (1 << PA0);
        PORTC = segmentDigits[hoursTens];
        _delay_ms(2);

        PORTA &= ~(1 << PA4) & ~(1 << PA3) & ~(1 << PA2) & ~(1 << PA5) & ~(1 << PA0);
        PORTA |= (1 << PA1);
        PORTC = segmentDigits[hoursUnits];
        _delay_ms(2);

        if (interruptFlag == 1)
        {
            incrementSegment();
            interruptFlag = 0;
        }
    }
}

void Timer1CTCInit(void)
{

    TCNT1 = 0;


    TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS12);

    TCCR1A = (1 << FOC1A);

    /*Set the compare match value equals 1000 to make 1 second delay =1000 milisecond*/
    OCR1A = 1000;

    /* Enable Timer1 Compare A Interrupt */
    TIMSK |= (1 << OCIE1A);
}

void INT0Init(void)
{
    //make the connected pin to push button as input pin
    DDRD &= (~(1 << PD2));
    //Activate the internal pull up
    PORTD |= (1 << PD2);

    //Enable External interrupt Request 0
    MCUCR |= (1 << ISC01);
    MCUCR &= ~(1 << ISC00);

    GIFR |= (1 << INTF0);
    //Enable External interrupt 0
    GICR |= (1 << INT0);
}
void INT1Init(void)
{
    //External pull down (no need to activate the internal pull up)
    DDRD &= ~(1 << PD3);

    //The Raising edge of INT1 generates an interrupt request
    MCUCR |= (1 << ISC10) | (1 << ISC11);

    GIFR |= (1 << INTF1);
    //Enable External interrupt 1
    GICR |= (1 << INT1);
}
void INT2Init(void)
{

    DDRB &= ~(1 << PB2);
    //Activate the internal pull up
    PORTB |= (1 << PB2);


    MCUCSR &= ~(1 << ISC2);

    GIFR |= (1 << INTF2);

    //Enable External interrupt 2
    GICR |= (1 << INT2);
}

void SevenSegmentInit(void)
{
    //set first 4 pins in c as output pins which are connected to 7447
    DDRC |= 0x0F;

    //set 4 pins at the beginning(Common Anode On with Zero/OFF with 1)
    PORTC &= 0xF0;
}

void incrementSegment(void)
{
    seconds++;

    if (seconds > 59)
    {
        seconds = 0;
        minutes++;
    }
    if (minutes > 59)
    {
        minutes = 0;
        hours++;
    }
    if (hours > 24)
    {
        hours = 0;
    }

    secondsTens = seconds / 10;
    secondsUnits = seconds - secondsTens * 10;

    minutesTens = minutes / 10;
    minutesUnits = minutes - minutesTens * 10;

    hoursTens = hours / 10;
    hoursUnits = hours - hoursTens * 10;
}
