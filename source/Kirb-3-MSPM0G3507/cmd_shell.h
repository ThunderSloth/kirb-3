#ifndef CMD_SHELL_H
#define CMD_SHELL_H

/** @file cmd_shell.h
 *  @brief Interactive debug-shell interface on the MSPM0 debug UART.
 */

/** Enable the debug UART and clear the command buffer. */
void cmd_shell_init(void);

/** Print the boot banner and initial prompt. */
void cmd_shell_print_boot_msg(void);

/** Poll the debug UART and execute completed command lines. */
void cmd_shell_poll(void);

#endif // CMD_SHELL_H
