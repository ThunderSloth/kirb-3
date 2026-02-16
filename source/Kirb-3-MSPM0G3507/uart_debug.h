#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include <stdbool.h> // for bool
#include <stdint.h>

/*
Command to connect to serial consol on mac:
screen /dev/tty.usbmodemMG3500011 115200
*/

// Initialize debug UART, if needed.
void UART_debug_init(void);

char UART_debug_getc(void);
bool UART_debug_available(void);

// Low-level: send one character
void UART_debug_putc(char c);

// Mid-level: send strings
void UART_debug_print(const char *s);
void UART_debug_println(const char *s);

// printf-style debug output
void UART_debug_printf(const char *fmt, ...);

#endif // UART_DEBUG_H_
