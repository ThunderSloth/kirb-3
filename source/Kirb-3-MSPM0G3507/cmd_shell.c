#include "cmd_shell.h"

#include "cmd_core.h"
#include "kirb-3.h"
#include "uart.h"

#include <stdint.h>

/** @file cmd_shell.c
 *  @brief Line-editing debug shell for local serial diagnostics.
 */

#define CMD_SHELL_BUF_LEN (96u)
#define ONE_MHZ (1000000UL)
#define KHZ_IN_MHZ (1000UL)

static char g_cmd_buf[CMD_SHELL_BUF_LEN];
static uint8_t g_cmd_len = 0u;

/** Adapt command-core response lines to the debug UART. */
static void cmd_shell_write(void *ctx, const char *text)
{
    (void)ctx;
    uart_println(UART_DEBUG_INST, text);
}

/** Emit the interactive shell prompt. */
static void cmd_shell_print_prompt(void)
{
    uart_print(UART_DEBUG_INST, "> ");
}

/** Enable the debug shell UART and clear the current line buffer. */
void cmd_shell_init(void)
{
    uart_enable(UART_DEBUG_INST);
    g_cmd_len = 0u;
}

/** Print the firmware boot banner and first shell prompt. */
void cmd_shell_print_boot_msg(void)
{
    const uint32_t mhz = CPUCLK_FREQ / ONE_MHZ;
    const uint32_t frac = (CPUCLK_FREQ % ONE_MHZ) / KHZ_IN_MHZ;

    uart_printf(UART_DEBUG_INST, "Kirb-3 booting at %lu.%03lu MHz...\n",
                (unsigned long)mhz, (unsigned long)frac);
    uart_println(UART_DEBUG_INST, "Type 'help' for command list.");
    cmd_shell_print_prompt();
}

/** Poll the debug UART, edit the current line, and execute completed commands. */
void cmd_shell_poll(void)
{
    static const CmdOutput shell_out = {
        .write = cmd_shell_write,
        .ctx = 0,
    };

    while (uart_available(UART_DEBUG_INST))
    {
        char ch = uart_getc(UART_DEBUG_INST);

        if ((uint8_t)ch == 0x7Fu)
        {
            ch = '\b';
        }

        if ((ch == '\r') || (ch == '\n'))
        {
            uart_println(UART_DEBUG_INST, "");

            if (g_cmd_len != 0u)
            {
                g_cmd_buf[g_cmd_len] = '\0';
                (void)cmd_core_execute(g_cmd_buf, &shell_out);
                g_cmd_len = 0u;
            }

            cmd_shell_print_prompt();
            continue;
        }

        if (ch == '\b')
        {
            if (g_cmd_len != 0u)
            {
                g_cmd_len--;
                uart_print(UART_DEBUG_INST, "\b \b");
            }
            continue;
        }

        if ((g_cmd_len + 1u) >= CMD_SHELL_BUF_LEN)
        {
            uart_println(UART_DEBUG_INST, "");
            uart_println(UART_DEBUG_INST, "ERR: command too long");
            g_cmd_len = 0u;
            cmd_shell_print_prompt();
            continue;
        }

        g_cmd_buf[g_cmd_len++] = ch;
        uart_putc(UART_DEBUG_INST, ch);
    }
}
