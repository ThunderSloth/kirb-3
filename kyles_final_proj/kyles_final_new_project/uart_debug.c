#include "uart_debug.h"

#include "ti_msp_dl_config.h" // for UART_DEBUG_INST

#include <stdarg.h>
#include <stdio.h>
#include <ti/driverlib/driverlib.h>

#define UART_DEBUG_BUF_SIZE 128

void UART_debug_init(void)
{
    // SysConfig already configured UART0, but we ensure it's enabled.
    DL_UART_Main_enable(UART_DEBUG_INST);
}

void UART_debug_putc(char c)
{
    if (c == '\n')
    {
        DL_UART_Main_transmitData(UART_DEBUG_INST, (uint8_t) '\r');
        while (!DL_UART_Main_isTXFIFOEmpty(UART_DEBUG_INST))
        {
        }
    }

    DL_UART_Main_transmitData(UART_DEBUG_INST, (uint8_t) c);
    while (!DL_UART_Main_isTXFIFOEmpty(UART_DEBUG_INST))
    {
    }
}

void UART_debug_print(const char* s)
{
    while (*s)
    {
        UART_debug_putc(*s++);
    }
}

void UART_debug_println(const char* s)
{
    UART_debug_print(s);
    UART_debug_putc('\n');
}

char UART_debug_getc(void)
{
    while (DL_UART_Main_isRXFIFOEmpty(UART_DEBUG_INST))
    {
    }
    return (char) DL_UART_Main_receiveData(UART_DEBUG_INST);
}

bool UART_debug_available(void)
{
    return !DL_UART_Main_isRXFIFOEmpty(UART_DEBUG_INST);
}

void UART_debug_printf(const char* fmt, ...)
{
    char    buf[UART_DEBUG_BUF_SIZE];
    va_list args;

    va_start(args, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (n < 0)
        return;
    if (n > (int) sizeof(buf))
        n = (int) sizeof(buf);

    for (int i = 0; i < n; i++)
    {
        UART_debug_putc(buf[i]);
    }
}
