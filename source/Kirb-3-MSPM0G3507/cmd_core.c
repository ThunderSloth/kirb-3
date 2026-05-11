#include "cmd_core.h"

#include "battery.h"
#include "encoder.h"
#include "imu.h"
#include "motor.h"
#include "ping.h"
#include "rc.h"
#include "system_state.h"

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** @file cmd_core.c
 *  @brief Text command handlers for robot diagnostics and control toggles.
 */

#define CMD_CORE_LINE_BUF_SIZE (160u)

/** Handle ultrasonic summary and per-sensor diagnostic commands. */
static bool cmd_handle_ult(const CmdOutput *out, const char *args);

/** Print a compact ultrasonic pulse-width summary. */
static bool cmd_handle_ult_pw_summary(const CmdOutput *out);

/** Send one complete response line through a command output callback. */
static void cmd_write_line(const CmdOutput *out, const char *text)
{
    if ((out == NULL) || (out->write == NULL))
    {
        return;
    }

    out->write(out->ctx, text);
}

/** Format and send one complete response line. */
static void cmd_writef(const CmdOutput *out, const char *fmt, ...)
{
    char line[CMD_CORE_LINE_BUF_SIZE];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(line, sizeof(line), fmt, args);
    va_end(args);

    cmd_write_line(out, line);
}

/** Print the available command list. */
static void cmd_write_help(const CmdOutput *out)
{
    cmd_write_line(out, "Commands:");
    cmd_write_line(out, "  help            - Show command list");
    cmd_write_line(out, "  ping            - Return pong");
    cmd_write_line(out, "  status          - Show drive/mode/fault summary");
    cmd_write_line(out, "  battery         - Show latest battery sample");
    cmd_write_line(out,
                   "  imu             - Show IMU status and cached sample");
    cmd_write_line(out, "  ult             - Show all ultrasonic readings");
    cmd_write_line(out, "  ult N           - Show one ultrasonic sensor");
    cmd_write_line(out, "  rc              - Show RC channel pulse widths");
    cmd_write_line(out, "  motor           - Show motor PWM outputs");
    cmd_write_line(out, "  servo           - Show servo PWM outputs");
    cmd_write_line(out, "  encoder         - Show encoder state");
    cmd_write_line(out, "  globals         - Show core globals summary");
    cmd_write_line(out, "  enable          - Enable drive output");
    cmd_write_line(out, "  disable         - Disable drive output");
}

/** Handle the ping liveness command. */
static bool cmd_handle_ping(const CmdOutput *out)
{
    cmd_write_line(out, "pong");
    return true;
}

/** Print high-level robot state and battery summary. */
static bool cmd_handle_status(const CmdOutput *out)
{
    cmd_writef(out, "Drive: %s",
               system_state_drive_enabled() ? "ENABLED" : "DISABLED");
    cmd_writef(out, "Mode: %s",
               system_state_mode_name(system_state_control_mode()));
    cmd_writef(out, "Faults: 0x%08lx",
               (unsigned long)system_state_fault_flags());
    cmd_writef(out, "Battery: raw=%u mv=%u", (unsigned)g_battery_adc_raw,
               (unsigned)g_battery_mv);
    return true;
}

/** Print the latest battery ADC and voltage readings. */
static bool cmd_handle_battery(const CmdOutput *out)
{
    cmd_writef(out, "Battery: raw=%u mv=%u", (unsigned)g_battery_adc_raw,
               (unsigned)g_battery_mv);
    return true;
}

/** Print IMU presence, identity, and cached motion samples. */
static bool cmd_handle_imu(const CmdOutput *out)
{
    if (!g_imu_present && !imu_read_who_am_i())
    {
        cmd_write_line(out, "IMU: not present");
        return false;
    }

    cmd_writef(out, "IMU: present=YES addr=0x%02X who_am_i=0x%02X match=%s",
               (unsigned)g_imu_i2c_addr, (unsigned)g_imu_who_am_i,
               (g_imu_who_am_i == IMU_WHO_AM_I_EXPECTED) ? "YES" : "NO");
    cmd_writef(out, "IMU raw accel: x=%d y=%d z=%d", (int)g_imu_accel_raw[0],
               (int)g_imu_accel_raw[1], (int)g_imu_accel_raw[2]);
    cmd_writef(out, "IMU raw gyro:  x=%d y=%d z=%d", (int)g_imu_gyro_raw[0],
               (int)g_imu_gyro_raw[1], (int)g_imu_gyro_raw[2]);
    cmd_writef(out, "IMU accel g:   x=%.3f y=%.3f z=%.3f",
               (double)g_imu_accel_mg[0] / 1000.0,
               (double)g_imu_accel_mg[1] / 1000.0,
               (double)g_imu_accel_mg[2] / 1000.0);
    cmd_writef(out, "IMU gyro dps:  x=%.3f y=%.3f z=%.3f",
               (double)g_imu_gyro_mdps[0] / 1000.0,
               (double)g_imu_gyro_mdps[1] / 1000.0,
               (double)g_imu_gyro_mdps[2] / 1000.0);
    return true;
}

/** Print all cached RC input pulse widths. */
static bool cmd_handle_rc(const CmdOutput *out)
{
    cmd_writef(
        out, "RC: ch1=%u ch2=%u ch3=%u ch4=%u ch5=%u ch6=%u",
        (unsigned)g_rc_pw_us[RC_CH1_IDX], (unsigned)g_rc_pw_us[RC_CH2_IDX],
        (unsigned)g_rc_pw_us[RC_CH3_IDX], (unsigned)g_rc_pw_us[RC_CH4_IDX],
        (unsigned)g_rc_pw_us[RC_CH5_IDX], (unsigned)g_rc_pw_us[RC_CH6_IDX]);
    return true;
}

/** Print left and right motor output pulse widths. */
static bool cmd_handle_motor(const CmdOutput *out)
{
    cmd_writef(out, "Motor: left=%u right=%u", (unsigned)g_mtr_pw_us[L_MTR_IDX],
               (unsigned)g_mtr_pw_us[R_MTR_IDX]);
    return true;
}

/** Print pan and tilt servo output pulse widths. */
static bool cmd_handle_servo(const CmdOutput *out)
{
    cmd_writef(out, "Servo: pan=%u tilt=%u",
               (unsigned)g_servo_pw_us[PAN_SERVO_IDX],
               (unsigned)g_servo_pw_us[TILT_SERVO_IDX]);
    return true;
}

/** Print encoder packets, counts, success totals, and error totals. */
static bool cmd_handle_encoder(const CmdOutput *out)
{
    cmd_writef(out, "Encoder RX: done=%s packet=[0x%02X 0x%02X]",
               g_encoder_rx_done ? "true" : "false",
               (unsigned)g_encoder_rx_packet[ENCODER_RX_HIGH_BYTE_IDX],
               (unsigned)g_encoder_rx_packet[ENCODER_RX_LOW_BYTE_IDX]);
    cmd_writef(out,
               "Encoder 0: valid=%s count=%d packet=[0x%02X 0x%02X] ok=%lu errors=%lu",
               g_encoder_valid[ENCODER1_IDX] ? "true" : "false",
               (int)g_encoder_count[ENCODER1_IDX],
               (unsigned)g_encoder_last_packet[ENCODER1_IDX]
                                               [ENCODER_RX_HIGH_BYTE_IDX],
               (unsigned)g_encoder_last_packet[ENCODER1_IDX]
                                               [ENCODER_RX_LOW_BYTE_IDX],
               (unsigned long)g_encoder_success_count[ENCODER1_IDX],
               (unsigned long)g_encoder_error_count[ENCODER1_IDX]);
    cmd_writef(out,
               "Encoder 1: valid=%s count=%d packet=[0x%02X 0x%02X] ok=%lu errors=%lu",
               g_encoder_valid[ENCODER2_IDX] ? "true" : "false",
               (int)g_encoder_count[ENCODER2_IDX],
               (unsigned)g_encoder_last_packet[ENCODER2_IDX]
                                               [ENCODER_RX_HIGH_BYTE_IDX],
               (unsigned)g_encoder_last_packet[ENCODER2_IDX]
                                               [ENCODER_RX_LOW_BYTE_IDX],
               (unsigned long)g_encoder_success_count[ENCODER2_IDX],
               (unsigned long)g_encoder_error_count[ENCODER2_IDX]);
    return true;
}

/** Print a combined snapshot of the major firmware globals. */
static bool cmd_handle_globals(const CmdOutput *out)
{
    (void)cmd_handle_status(out);
    (void)cmd_handle_rc(out);
    (void)cmd_handle_imu(out);
    (void)cmd_handle_ult_pw_summary(out);
    (void)cmd_handle_motor(out);
    (void)cmd_handle_servo(out);
    (void)cmd_handle_encoder(out);
    return true;
}

/** Print a compact ultrasonic pulse-width summary. */
static bool cmd_handle_ult_pw_summary(const CmdOutput *out)
{
    cmd_writef(out,
               "ULT pw_us: u0=%lu u1=%lu u2=%lu u3=%lu u4=%lu u5=%lu u6=%lu "
               "u7=%lu u8=%lu u9=%lu",
               (unsigned long)g_ult_pw_us[0], (unsigned long)g_ult_pw_us[1],
               (unsigned long)g_ult_pw_us[2], (unsigned long)g_ult_pw_us[3],
               (unsigned long)g_ult_pw_us[4], (unsigned long)g_ult_pw_us[5],
               (unsigned long)g_ult_pw_us[6], (unsigned long)g_ult_pw_us[7],
               (unsigned long)g_ult_pw_us[8], (unsigned long)g_ult_pw_us[9]);
    return true;
}

/** Handle ultrasonic summary and per-sensor diagnostic commands. */
static bool cmd_handle_ult(const CmdOutput *out, const char *args)
{
    const char *p = args;

    while (*p == ' ')
    {
        p++;
    }

    if (*p == '\0')
    {
        cmd_write_line(out, "ULT readings:");
        cmd_write_line(out, "Idx | Enabled |   PW (us) |    cm     |     m     "
                            "|    in    |    ft");
        cmd_write_line(out, "----+---------+-----------+-----------+-----------"
                            "+----------+----------");

        for (int i = 0; i < ULT_COUNT; i++)
        {
            if (!g_ult[i].enabled)
            {
                cmd_writef(out,
                           "%3d |   NO    |  DISABLED |  DISABLED |  DISABLED "
                           "| DISABLED | DISABLED",
                           i);
                continue;
            }

            const uint32_t pw_us = g_ult_pw_us[i];
            cmd_writef(
                out, "%3d |   YES   | %9lu | %9.2f | %9.3f | %8.2f | %8.2f", i,
                (unsigned long)pw_us, ping_us_to_cm_float((float)pw_us),
                ping_us_to_m_float((float)pw_us),
                ping_us_to_in_float((float)pw_us),
                ping_us_to_ft_float((float)pw_us));
        }
        return true;
    }

    char *endptr = NULL;
    const long idx = strtol(p, &endptr, 10);

    if ((p == endptr) || (*endptr != '\0'))
    {
        cmd_write_line(out, "ERR: usage: ult [idx]");
        return false;
    }

    if ((idx < 0) || (idx >= ULT_COUNT))
    {
        cmd_writef(out, "ERR: idx out of range (0..%d)", ULT_COUNT - 1);
        return false;
    }

    if (!g_ult[idx].enabled)
    {
        cmd_writef(out, "ULT %ld: DISABLED", idx);
        return true;
    }

    {
        const uint32_t pw_us = g_ult_pw_us[idx];
        cmd_writef(out,
                   "ULT %ld: enabled=YES angle=%.1f pw_us=%lu cm=%.2f m=%.3f "
                   "in=%.2f ft=%.2f",
                   idx, g_ult[idx].angle_deg, (unsigned long)pw_us,
                   ping_us_to_cm_float((float)pw_us),
                   ping_us_to_m_float((float)pw_us),
                   ping_us_to_in_float((float)pw_us),
                   ping_us_to_ft_float((float)pw_us));
    }

    return true;
}

/** Enable drive output from the command interface. */
static bool cmd_handle_enable(const CmdOutput *out)
{
    system_state_set_drive_enabled(true);
    cmd_write_line(out, "Drive enabled");
    return true;
}

/** Disable drive output and force cached motor commands to neutral. */
static bool cmd_handle_disable(const CmdOutput *out)
{
    system_state_set_drive_enabled(false);
    g_mtr_pw_us[L_MTR_IDX] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    g_mtr_pw_us[R_MTR_IDX] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    cmd_write_line(out, "Drive disabled");
    return true;
}

/** Parse a command line and dispatch it to the matching command handler. */
bool cmd_core_execute(const char *line, const CmdOutput *out)
{
    if (line == NULL)
    {
        cmd_write_line(out, "ERR: null command");
        return false;
    }

    while (*line == ' ')
    {
        line++;
    }

    if (*line == '\0')
    {
        return true;
    }

    if ((strcmp(line, "help") == 0) || (strcmp(line, "?") == 0))
    {
        cmd_write_help(out);
        return true;
    }

    if (strcmp(line, "ping") == 0)
    {
        return cmd_handle_ping(out);
    }

    if (strcmp(line, "status") == 0)
    {
        return cmd_handle_status(out);
    }

    if (strcmp(line, "battery") == 0)
    {
        return cmd_handle_battery(out);
    }

    if (strcmp(line, "imu") == 0)
    {
        return cmd_handle_imu(out);
    }

    if (strcmp(line, "rc") == 0)
    {
        return cmd_handle_rc(out);
    }

    if ((strcmp(line, "motor") == 0) || (strcmp(line, "mtr") == 0))
    {
        return cmd_handle_motor(out);
    }

    if (strcmp(line, "servo") == 0)
    {
        return cmd_handle_servo(out);
    }

    if (strcmp(line, "encoder") == 0)
    {
        return cmd_handle_encoder(out);
    }

    if (strcmp(line, "globals") == 0)
    {
        return cmd_handle_globals(out);
    }

    if (strcmp(line, "enable") == 0)
    {
        return cmd_handle_enable(out);
    }

    if (strcmp(line, "disable") == 0)
    {
        return cmd_handle_disable(out);
    }

    if (strncmp(line, "ult", 3) == 0)
    {
        return cmd_handle_ult(out, line + 3);
    }

    cmd_writef(out, "ERR: unknown command: %s", line);
    return false;
}
