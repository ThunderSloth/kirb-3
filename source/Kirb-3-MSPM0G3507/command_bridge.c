#include "command_bridge.h"

#include "cmd_core.h"
#include "kirb-3.h"
#include "uart.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/** @file command_bridge.c
 *  @brief Line-oriented command transport for the ESP32 dashboard UART.
 */

#define COMMAND_BRIDGE_LINE_BUF_SIZE (96u)
#define COMMAND_BRIDGE_RX_BUF_SIZE (256u)

static char g_cmd_line[COMMAND_BRIDGE_LINE_BUF_SIZE];
static uint32_t g_cmd_line_len = 0u;
static volatile char g_cmd_rx_buf[COMMAND_BRIDGE_RX_BUF_SIZE];
static volatile uint16_t g_cmd_rx_head = 0u;
static volatile uint16_t g_cmd_rx_tail = 0u;

/** Pop one byte from the ISR-filled command ring buffer. */
static bool command_bridge_rx_pop(char *ch)
{
    bool has_data = false;

    __disable_irq();
    if (g_cmd_rx_head != g_cmd_rx_tail)
    {
        *ch = g_cmd_rx_buf[g_cmd_rx_tail];
        g_cmd_rx_tail = (uint16_t)((g_cmd_rx_tail + 1u) %
                                   COMMAND_BRIDGE_RX_BUF_SIZE);
        has_data = true;
    }
    __enable_irq();

    return has_data;
}

/** Write one character to the ESP32 UART. */
static void uart_esp32_putc(char c)
{
    uart_putc(UART_ESP32_INST, c);
}

/** Write a string to the ESP32 UART. */
static void uart_esp32_print(const char *s)
{
    uart_print(UART_ESP32_INST, s);
}

/** Send one prefixed bridge response line, such as ACK/ERR/RSP. */
static void command_bridge_send_line(const char *prefix, const char *message)
{
    uart_esp32_print(prefix);
    uart_esp32_putc(',');
    uart_esp32_print(message);
    uart_esp32_putc('\n');
}

/** Adapt command-core output lines to bridge RSP records. */
static void command_bridge_core_write(void *ctx, const char *text)
{
    (void)ctx;
    command_bridge_send_line("RSP", text);
}

/** Validate and execute one complete bridge command line. */
static void command_bridge_handle_line(void)
{
    static const CmdOutput bridge_out = {
        .write = command_bridge_core_write,
        .ctx = 0,
    };

    bool ok;

    g_cmd_line[g_cmd_line_len] = '\0';

    if (strncmp(g_cmd_line, "CMD,", 4) != 0)
    {
        command_bridge_send_line("ERR", "bad command prefix");
        g_cmd_line_len = 0u;
        return;
    }

    ok = cmd_core_execute(&g_cmd_line[4], &bridge_out);
    command_bridge_send_line(ok ? "ACK" : "ERR", ok ? "ok" : "command failed");

    g_cmd_line_len = 0u;
}

/** Initialize the ESP32 command bridge UART and ring buffers. */
void command_bridge_init(void)
{
    g_cmd_line_len = 0u;
    g_cmd_rx_head = 0u;
    g_cmd_rx_tail = 0u;
    uart_enable(UART_ESP32_INST);
}

/** Drain received bridge bytes and execute complete command lines. */
void command_bridge_service(void)
{
    char ch;

    while (command_bridge_rx_pop(&ch))
    {
        if (ch == '\r')
        {
            continue;
        }

        if (ch == '\n')
        {
            if (g_cmd_line_len != 0u)
            {
                command_bridge_handle_line();
            }
            continue;
        }

        if ((g_cmd_line_len + 1u) >= COMMAND_BRIDGE_LINE_BUF_SIZE)
        {
            g_cmd_line_len = 0u;
            command_bridge_send_line("ERR", "command too long");
            continue;
        }

        g_cmd_line[g_cmd_line_len++] = ch;
    }
}

/** Copy received ESP32 UART bytes into the command bridge ring buffer. */
void command_bridge_uart_irq(void)
{
    while (uart_available(UART_ESP32_INST))
    {
        const char ch = uart_getc(UART_ESP32_INST);
        const uint16_t next_head =
            (uint16_t)((g_cmd_rx_head + 1u) % COMMAND_BRIDGE_RX_BUF_SIZE);

        if (next_head == g_cmd_rx_tail)
        {
            continue;
        }

        g_cmd_rx_buf[g_cmd_rx_head] = ch;
        g_cmd_rx_head = next_head;
    }
}
