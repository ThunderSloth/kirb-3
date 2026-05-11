#include "uart.h"

#include <stdarg.h>
#include <stdio.h>

/** @file uart.c
 *  @brief Small blocking UART print/read wrappers around DriverLib.
 */

#define UART_PRINTF_BUF_SIZE (128u)

/** Enable a UART peripheral through DriverLib. */
void uart_enable(UART_Regs *inst)
{
    DL_UART_Main_enable(inst);
}

/** Send one character, expanding newline to CRLF for terminal compatibility. */
void uart_putc(UART_Regs *inst, char c)
{
    if (c == '\n')
    {
        DL_UART_Main_transmitData(inst, (uint8_t)'\r');
        while (!DL_UART_Main_isTXFIFOEmpty(inst))
        {
        }
    }

    DL_UART_Main_transmitData(inst, (uint8_t)c);
    while (!DL_UART_Main_isTXFIFOEmpty(inst))
    {
    }
}

/** Send a null-terminated string over UART. */
void uart_print(UART_Regs *inst, const char *s)
{
    while (*s != '\0')
    {
        uart_putc(inst, *s++);
    }
}

/** Send a null-terminated string followed by a newline. */
void uart_println(UART_Regs *inst, const char *s)
{
    uart_print(inst, s);
    uart_putc(inst, '\n');
}

/** Block until one received character is available, then return it. */
char uart_getc(UART_Regs *inst)
{
    while (DL_UART_Main_isRXFIFOEmpty(inst))
    {
    }

    return (char)DL_UART_Main_receiveData(inst);
}

/** Return true when a UART receive byte is waiting. */
bool uart_available(UART_Regs *inst)
{
    return !DL_UART_Main_isRXFIFOEmpty(inst);
}

/** Format a short message into a local buffer and send it over UART. */
void uart_printf(UART_Regs *inst, const char *fmt, ...)
{
    char buf[UART_PRINTF_BUF_SIZE];
    va_list args;
    int n;

    va_start(args, fmt);
    n = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (n < 0)
    {
        return;
    }

    if (n > (int)sizeof(buf))
    {
        n = (int)sizeof(buf);
    }

    for (int i = 0; i < n; i++)
    {
        uart_putc(inst, buf[i]);
    }
}
