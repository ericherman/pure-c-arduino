#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stddef.h>

/*****************************************************************/
/* CONFIG params */
/*****************/
#ifndef Blink_delay_milliseconds
#define Blink_delay_milliseconds 500
#endif
/*****************************************************************/

/*****************************************************************/
/* PLATFORM #defines                                             */
/*                                                               */
/* On my Arduino Mega2560 LED_BUILTIN is PB7 (pin 7 of PORTB)    */
/*****************************************************************/
#ifndef DDR_LED_BUILTIN
#define DDR_LED_BUILTIN DDRB
#endif

#ifndef DD_LED_BUILTIN
#define DD_LED_BUILTIN DDB7
#endif

#ifndef PB_LED_BUILTIN
#define PB_LED_BUILTIN PORTB7
#endif

#ifndef PORT_LED_BUILTIN
#define PORT_LED_BUILTIN PORTB
#endif

#define OUTPUT 1
#define LOW 0
#define HIGH 1
/*****************************************************************/

/*****************************************************************/
/* PROTOTYPES */
/**************/
static void set_bit_u8(volatile uint8_t *b, unsigned index, int val);
/*****************************************************************/

/*****************************************************************/
/* PROGRAM */
/***********/
void setup(void)
{
	/* set LED_BUILTIN for output */
	set_bit_u8(&DDR_LED_BUILTIN, DD_LED_BUILTIN, OUTPUT);
}

void loop(void)
{
	set_bit_u8(&PORT_LED_BUILTIN, PB_LED_BUILTIN, LOW);
	_delay_ms(Blink_delay_milliseconds);

	set_bit_u8(&PORT_LED_BUILTIN, PB_LED_BUILTIN, HIGH);
	_delay_ms(Blink_delay_milliseconds);
}

int main(void)
{
	setup();
	for (;;) {
		loop();
	}
}

/*****************************************************************/

/*****************************************************************/
/* HELPER FUNCTIONS */
/********************/

/* let's avoid having to think carefully about bit twiddling syntax */
static void set_bit_u8(volatile uint8_t *b, unsigned index, int val)
{
	/* More concise should work, but seems too tricky: */
	/* val = val ? 1 : 0 */ ;
	/* *b ^= (-val ^ *b) & (1U << offset); */
	/* Instead, be a bit more verbose and trust the optimizer: */
	if (val) {
		*b |= (1U << index);
	} else {
		*b &= ~(1U << index);
	}
}

/*****************************************************************/
