#include "system_state.h"

/** @file system_state.c
 *  @brief Backing storage for the robot-wide control and fault state.
 */

static volatile bool g_drive_enabled = true;
static volatile ControlMode g_control_mode = CONTROL_MODE_RC;
static volatile uint32_t g_fault_flags = FAULT_NONE;

/** Initialize robot-wide state to boot defaults. */
void system_state_init(void)
{
    g_drive_enabled = true;
    g_control_mode = CONTROL_MODE_RC;
    g_fault_flags = FAULT_NONE;
}

/** Return true when drive outputs are currently enabled. */
bool system_state_drive_enabled(void)
{
    return g_drive_enabled;
}

/** Enable or disable drive outputs. */
void system_state_set_drive_enabled(bool enabled)
{
    g_drive_enabled = enabled;
}

/** Return the current high-level control mode. */
ControlMode system_state_control_mode(void)
{
    return g_control_mode;
}

/** Set the current high-level control mode. */
void system_state_set_control_mode(ControlMode mode)
{
    g_control_mode = mode;
}

/** Return the current fault bitmask. */
uint32_t system_state_fault_flags(void)
{
    return g_fault_flags;
}

/** Replace the current fault bitmask. */
void system_state_set_fault_flags(uint32_t fault_flags)
{
    g_fault_flags = fault_flags;
}

/** Return a short display name for a control mode. */
const char *system_state_mode_name(ControlMode mode)
{
    switch (mode)
    {
    case CONTROL_MODE_RC:
        return "RC";
    case CONTROL_MODE_WEB:
        return "WEB";
    case CONTROL_MODE_AUTO:
        return "AUTO";
    default:
        return "UNKNOWN";
    }
}
