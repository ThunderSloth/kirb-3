#ifndef CMD_CORE_H
#define CMD_CORE_H

/** @file cmd_core.h
 *  @brief Command parser shared by the debug shell and ESP32 bridge.
 */

#include <stdbool.h>

/** Output callback used by command handlers. */
typedef void (*CmdWriteFn)(void *ctx, const char *text);

/** Destination for command response lines. */
typedef struct
{
    CmdWriteFn write;
    void *ctx;
} CmdOutput;

/** Parse and execute one command line. */
bool cmd_core_execute(const char *line, const CmdOutput *out);

#endif // CMD_CORE_H
