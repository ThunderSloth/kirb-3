#ifndef UART_H_
#define UART_H_

/** @file uart.h
 *  @brief Blocking UART convenience helpers used by the MSPM0 firmware.
 */

#include <stdbool.h>
#include <stdint.h>

#include <ti/driverlib/driverlib.h>

/** Enable a UART peripheral. */
void uart_enable(UART_Regs *inst);

/** Blocking read of one received byte. */
char uart_getc(UART_Regs *inst);

/** Return true when at least one received byte is waiting. */
bool uart_available(UART_Regs *inst);

/** Blocking write of one byte, with LF expanded to CRLF. */
void uart_putc(UART_Regs *inst, char c);

/** Blocking write of a null-terminated string. */
void uart_print(UART_Regs *inst, const char *s);

/** Blocking write of a null-terminated string followed by a newline. */
void uart_println(UART_Regs *inst, const char *s);

/** Format a short string into a local buffer and send it over UART. */
void uart_printf(UART_Regs *inst, const char *fmt, ...);

#endif // UART_H_
