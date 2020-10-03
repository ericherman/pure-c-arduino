/* hello-serial.c: very basic firmware for an avr Arduino Mega2560
 * Copyright (c) 2020 Eric Herman <eric@freesa.org>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 */

#include <stddef.h>
#include <avr/io.h>
#include <avr/sfr_defs.h>	/* loop_until_bit_is_set */

/*****************************************************************/
/* PROTOTYPES */
/**************/
size_t our_strlen(const char *s);
char *ul_to_str(char *buf, size_t len, unsigned long z);
void serial_print_str(const char *buf, size_t len);
size_t serial_read_str(char *buf, size_t len);

static void uart_9600(void);
char uart_char_read(void);
void uart_char_send(char c);

/*****************************************************************/
/* PROGRAM */
/***********/
void setup(void)
{
	uart_9600();
	char *begin = "\n\n\n\nhello, world\n";
	serial_print_str(begin, our_strlen(begin));
}

unsigned long loop_cnt = 0;
void loop(void)
{
	const char *output;
	size_t buflen = 256;
	char buf[buflen];

	++loop_cnt;

	ul_to_str(buf, buflen, loop_cnt);
	serial_print_str(buf, buflen);

	output = " awaiting input: ";
	serial_print_str(output, our_strlen(output));

	serial_read_str(buf, buflen);

	output = "\nreceived: '";
	serial_print_str(output, our_strlen(output));
	serial_print_str(buf, our_strlen(buf));
	output = "'\n";
	serial_print_str(output, our_strlen(output));
}

int main(void)
{
	setup();
	for (;;) {
		loop();
	}
}

/*****************************************************************/
/* HELPER FUNCTIONS */
/********************/
void serial_print_str(const char *buf, size_t len)
{
	for (size_t i = 0; buf[i] && i < len; ++i) {
		if (buf[i] == '\n') {
			uart_char_send('\r');
		}
		uart_char_send(buf[i]);
	}
}

size_t serial_read_str(char *buf, size_t len)
{
	buf[0] = '\0';
	for (size_t i = 0; i < len;) {
		char c = uart_char_read();
		switch (c) {
		case '\r':
		case '\n':
			return i;
		default:
			buf[i++] = c;
			buf[i] = '\0';
		}
	}

	buf[len - 1] = '\0';
	return len;
}

char *ul_to_str(char *buf, size_t len, unsigned long z)
{
	size_t tmplen = 22;
	char tmp[tmplen];
	size_t i = 0;
	size_t j = 0;

	if (!buf || !len) {
		return NULL;
	} else if (len == 1) {
		buf[0] = '\0';
		return NULL;
	}

	if (!z) {
		buf[0] = '0';
		buf[1] = '\0';
		return buf;
	}

	for (i = 0; z && i < tmplen; ++i) {
		tmp[i] = '0' + (z % 10);
		z = z / 10;
	}
	for (j = 0; i && j < len; ++j, --i) {
		buf[j] = tmp[i - 1];
	}

	buf[j < len ? j : len - 1] = '\0';

	return buf;
}

// a DIY version of the standard C library "strlen" so that we do not need to
// pull in the bloat of a libc into our firmware just to get this function,
// this version is simple, even if it is perhaps a bit less efficient than the
// libc version.
size_t our_strlen(const char *s)
{
	if (!s) {
		return 0;
	}
	size_t i = 0;
	while (s[i]) {
		++i;
	}
	return i;
}

/*****************************************************************/
/* RAW UART */
/************/
/* http://www.nongnu.org/avr-libc/user-manual/group__util__setbaud.html */

#ifndef F_CPU
#define F_CPU 4000000
#endif

#ifndef UBRRH
#ifdef UBRR0H
#define UBRRH UBRR0H
#endif
#endif

#ifndef UBRRL
#ifdef UBRR0L
#define UBRRL UBRR0L
#endif
#endif

#ifndef UCSRA
#ifdef UCSR0A
#define UCSRA UCSR0A
#endif
#endif

#ifndef UDR
#ifdef UDR0
#define UDR UDR0
#endif
#endif

#ifndef UDRE
#ifdef UDRE0
#define UDRE UDRE0
#endif
#endif

#ifndef RXC
#ifdef RXC0
#define RXC RXC0
#endif
#endif

#ifndef U2X
#ifdef U2X0
#define U2X U2X0
#endif
#endif

static void uart_9600(void)
{
#undef BAUD			// avoid compiler warning
#define BAUD 9600
#include <util/setbaud.h>
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif
#undef BAUD
}

void uart_char_send(char c)
{
	loop_until_bit_is_set(UCSRA, UDRE);
	UDR = c;
}

char uart_char_read(void)
{
	loop_until_bit_is_set(UCSRA, RXC);
	return UDR;
}

/*****************************************************************/
