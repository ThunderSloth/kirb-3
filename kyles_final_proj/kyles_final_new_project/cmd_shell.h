#ifndef CMD_SHELL_H_
#define CMD_SHELL_H_

// Show boot banner and menu (call once after UART init)
void cmd_shell_print_boot_msg(void);

// Poll for incoming characters and process complete commands
void cmd_shell_poll(void);

#endif // CMD_SHELL_H_
