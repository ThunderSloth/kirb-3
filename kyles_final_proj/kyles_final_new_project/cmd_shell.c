#include "cmd_shell.h"

#include "kyles_new_kirb-3.h"
#include "ping.h"
#include "uart_debug.h"

#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define ONE_MHZ 1000000UL
#define KHZ_IN_MHZ 1000UL

#define CMD_BUF_LEN 64

static char cmd_buf[CMD_BUF_LEN];
static uint8_t cmd_len = 0;

// internal helpers, not in header
static void print_menu(void);
static void print_prompt(void);
static void process_command(const char *cmd);

// per-command handlers
static void handle_help_command(void);
static void handle_ping_command(void);
static void handle_ult_command(const char *args);

void cmd_shell_print_boot_msg(void)
{
    uint32_t mhz = CPUCLK_FREQ / ONE_MHZ;
    uint32_t frac = (CPUCLK_FREQ % ONE_MHZ) / KHZ_IN_MHZ; // 3 decimal digits

    UART_debug_printf("Kirb-3 booting at %lu.%03lu MHz...\n",
                      (unsigned long)mhz, (unsigned long)frac);

    print_menu();
}

static void print_prompt(void)
{
    UART_debug_printf("> ");
}

static void print_menu(void)
{
    UART_debug_printf(
        "\n================ Kirb-3 Command Menu ================\n");
    UART_debug_printf("  help            - Show this menu\n");
    UART_debug_printf("  ping            - Test command (prints 'pong')\n");
    UART_debug_printf("  ult             - Show all ultrasonic readings\n");
    UART_debug_printf(
        "  ult N           - Show ultrasonic reading for sensor N\n");
    UART_debug_printf(
        "=======================================================\n\n");
    print_prompt();
}

// -----------------------------------------------------------------------------
// Command handlers
// -----------------------------------------------------------------------------

static void handle_help_command(void)
{
    print_menu();
}

static void handle_ping_command(void)
{
    UART_debug_printf("pong\n");
}

static void handle_ult_command(const char *args)
{
    const char *p = args;

    // Skip spaces after "ult"
    while (*p == ' ')
    {
        p++;
    }

    // =========================================================================
    // Case 1: "ult" → print all sensors in a nicely aligned table
    // =========================================================================
    if (*p == '\0')
    {

        UART_debug_printf("ULT readings:\n");
        UART_debug_printf("Idx | Enabled |   PW(us)  |    cm    |     m     |  "
                          "  in    |    ft\n");
        UART_debug_printf("----+---------+-----------+-----------+-----------+-"
                          "---------+----------\n");

        for (int i = 0; i < ULT_COUNT; i++)
        {

            if (!g_ult[i].enabled)
            {
                UART_debug_printf("%3d |   NO   |  DISABLED |  DISABLED |  "
                                  "DISABLED | DISABLED | DISABLED\n",
                                  i);
                continue;
            }

            uint32_t pw_us = g_ult_pw_us[i];

            float cm = ping_us_to_cm_float((float)pw_us);
            float m = ping_us_to_m_float((float)pw_us);
            float in = ping_us_to_in_float((float)pw_us);
            float ft = ping_us_to_ft_float((float)pw_us);

            UART_debug_printf(
                "%3d |  YES   | %9u | %9.2f | %10.3f | %8.2f | %8.2f\n", i,
                (unsigned)pw_us, cm, m, in, ft);
        }
        return;
    }

    // =========================================================================
    // Case 2: "ult N" → specific sensor readout
    // =========================================================================

    char *endptr = NULL;
    long idx = strtol(p, &endptr, 10);

    if (p == endptr || *endptr != '\0')
    {
        UART_debug_printf("ERR: usage: ult [idx]\n");
        return;
    }

    if (idx < 0 || idx >= ULT_COUNT)
    {
        UART_debug_printf("ERR: idx out of range (0..%d)\n", ULT_COUNT - 1);
        return;
    }

    // If sensor is disabled, print only that status
    if (!g_ult[idx].enabled)
    {
        UART_debug_printf("ULT sensor %ld: DISABLED\n", idx);
        return;
    }

    uint32_t pw_us = g_ult_pw_us[idx];

    float cm = ping_us_to_cm_float((float)pw_us);
    float m = ping_us_to_m_float((float)pw_us);
    float in = ping_us_to_in_float((float)pw_us);
    float ft = ping_us_to_ft_float((float)pw_us);

    UART_debug_printf("ULT sensor %ld:\n", idx);
    UART_debug_printf("  Enabled     : YES\n");
    UART_debug_printf("  Angle (deg) : %.1f\n", g_ult[idx].angle_deg);
    UART_debug_printf("  Pulse Width : %u us\n", (unsigned)pw_us);
    UART_debug_printf("  Metric      : %.2f cm, %.3f m\n", cm, m);
    UART_debug_printf("  Imperial    : %.2f in, %.2f ft\n", in, ft);
}

// -----------------------------------------------------------------------------
// Core dispatcher
// -----------------------------------------------------------------------------

static void process_command(const char *cmd)
{
    // Skip leading spaces just in case
    while (*cmd == ' ')
    {
        cmd++;
    }

    // empty line
    if (*cmd == '\0')
    {
        return;
    }

    // "help" or "?"
    if ((strcmp(cmd, "help") == 0) || (strcmp(cmd, "?") == 0))
    {
        handle_help_command();
        return;
    }

    // "ping"
    if (strcmp(cmd, "ping") == 0)
    {
        handle_ping_command();
        return;
    }

    // "ult" or "ult N"
    if (strncmp(cmd, "ult", 3) == 0)
    {
        handle_ult_command(cmd + 3);
        return;
    }

    // Unknown command
    UART_debug_printf("Unknown command: %s\n", cmd);
}

// -----------------------------------------------------------------------------
// Shell polling
// -----------------------------------------------------------------------------

void cmd_shell_poll(void)
{
    while (UART_debug_available())
    {
        char c = UART_debug_getc();
        uint8_t uc = (uint8_t)c;

        // Normalize DEL (0x7F) to backspace like the lab only handled '\b'
        if (uc == 0x7F)
        {
            c = '\b';
        }

        // Echo whatever we got (like UART_out_char in the lab)
        UART_debug_putc(c);

        if (c == '\r' || c == '\n')
        {
            // End of line: terminate buffer and process if non-empty
            if (cmd_len > 0)
            {
                cmd_buf[cmd_len] = '\0';
                UART_debug_putc('\n'); // move to next line for output
                process_command(cmd_buf);
                cmd_len = 0;

                print_prompt();
            }
            // If cmd_len == 0, just ignore blank lines
        }
        else if (c == '\b')
        {
            // Backspace: only adjust buffer index, do NOT store '\b'
            if (cmd_len > 0)
            {
                cmd_len--;
            }
        }
        else
        {
            // Normal character: store if there is room
            if (cmd_len < CMD_BUF_LEN - 1)
            {
                cmd_buf[cmd_len++] = c;
            }
            else
            {
                // Optional: you could beep or ignore here
                // UART_debug_putc('\a');
            }
        }
    }
}
