#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

/** @file system_state.h
 *  @brief Shared high-level robot mode, drive-enable, and fault state.
 */

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    CONTROL_MODE_RC = 0,
    CONTROL_MODE_WEB,
    CONTROL_MODE_AUTO,
} ControlMode;

typedef enum
{
    FAULT_NONE = 0,
    FAULT_LINK_LOSS = (1u << 0),
    FAULT_RC_INVALID = (1u << 1),
    FAULT_INTERNAL = (1u << 2),
} FaultFlag;

/** Initialize mode, drive-enable, and fault flags to their boot defaults. */
void system_state_init(void);

/** Return true when drive PWM outputs are allowed to follow commands. */
bool system_state_drive_enabled(void);

/** Enable or disable drive PWM outputs. */
void system_state_set_drive_enabled(bool enabled);

/** Return the current top-level control source. */
ControlMode system_state_control_mode(void);

/** Set the current top-level control source. */
void system_state_set_control_mode(ControlMode mode);

/** Return the current fault bitmask. */
uint32_t system_state_fault_flags(void);

/** Replace the current fault bitmask. */
void system_state_set_fault_flags(uint32_t fault_flags);

/** Convert a control mode enum into a short display string. */
const char *system_state_mode_name(ControlMode mode);

#endif // SYSTEM_STATE_H
