#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#include <stdbool.h> // for bool
#include <stdint.h>

// Initialize debug UART, if needed.
void UART_debug_init(void);

char UART_debug_getc(void);
bool UART_debug_available(void);

// Low-level: send one character
void UART_debug_putc(char c);

// Mid-level: send strings
void UART_debug_print(const char* s);
void UART_debug_println(const char* s);

// printf-style debug output
void UART_debug_printf(const char* fmt, ...);

// Optional: if you *really* want to globally remap printf,
// you *can* leave this here, but be aware it's global.
//
// #define printf UART_debug_printf

// Convenience macro so you can just write DBG_PRINTF(...)
#define DBG_PRINTF(...) UART_debug_printf(__VA_ARGS__)

#endif // UART_DEBUG_H_
