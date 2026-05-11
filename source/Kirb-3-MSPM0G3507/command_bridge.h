#ifndef COMMAND_BRIDGE_H
#define COMMAND_BRIDGE_H

/** @file command_bridge.h
 *  @brief UART command bridge between the ESP32 dashboard and MSPM0 commands.
 */

/** Enable the ESP32 UART command path and clear its buffers. */
void command_bridge_init(void);

/** Drain queued ESP32 bytes and execute complete command lines. */
void command_bridge_service(void);

/** Handle receive interrupts from the ESP32 UART. */
void command_bridge_uart_irq(void);

#endif // COMMAND_BRIDGE_H
