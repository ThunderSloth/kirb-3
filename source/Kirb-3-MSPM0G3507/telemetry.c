#include "telemetry.h"

#include "battery.h"
#include "encoder.h"
#include "imu.h"
#include "motor.h"
#include "ping.h"
#include "rc.h"
#include "uart.h"

#include <stdbool.h>
#include <stdio.h>

/** @file telemetry.c
 *  @brief CSV telemetry frame builder for the ESP32 dashboard bridge.
 */

static volatile bool g_telem_due = false;

/** Write one character to the ESP32 telemetry UART. */
static void uart_esp32_putc(char c)
{
    uart_putc(UART_ESP32_INST, c);
}

/** Write a string to the ESP32 telemetry UART. */
static void uart_esp32_print(const char *s)
{
    uart_print(UART_ESP32_INST, s);
}

/** Snapshot shared globals and emit one TEL CSV frame. */
static void telemetry_send_frame(void)
{
    char frame[448];

    const uint16_t rc_snapshot[RC_CH_COUNT] = {
        g_rc_pw_us[RC_CH_LS_Y], g_rc_pw_us[RC_CH_LS_X], g_rc_pw_us[RC_CH_RS_Y],
        g_rc_pw_us[RC_CH_RS_X], g_rc_pw_us[RC_CH_VR_A], g_rc_pw_us[RC_CH_VR_B],
    };
    const uint16_t ult_snapshot[ULT_COUNT] = {
        g_ult_pw_us[ULT0_IDX], g_ult_pw_us[ULT1_IDX], g_ult_pw_us[ULT2_IDX],
        g_ult_pw_us[ULT3_IDX], g_ult_pw_us[ULT4_IDX], g_ult_pw_us[ULT5_IDX],
        g_ult_pw_us[ULT6_IDX], g_ult_pw_us[ULT7_IDX], g_ult_pw_us[ULT8_IDX],
        g_ult_pw_us[ULT9_IDX],
    };
    const uint16_t ult_enabled_snapshot[ULT_COUNT] = {
        g_ult[ULT0_IDX].enabled ? 1u : 0u, g_ult[ULT1_IDX].enabled ? 1u : 0u,
        g_ult[ULT2_IDX].enabled ? 1u : 0u, g_ult[ULT3_IDX].enabled ? 1u : 0u,
        g_ult[ULT4_IDX].enabled ? 1u : 0u, g_ult[ULT5_IDX].enabled ? 1u : 0u,
        g_ult[ULT6_IDX].enabled ? 1u : 0u, g_ult[ULT7_IDX].enabled ? 1u : 0u,
        g_ult[ULT8_IDX].enabled ? 1u : 0u, g_ult[ULT9_IDX].enabled ? 1u : 0u,
    };
    const uint16_t mtr_snapshot[MOTOR_COUNT] = {g_mtr_pw_us[L_MTR_IDX],
                                                g_mtr_pw_us[R_MTR_IDX]};
    const uint16_t servo_snapshot[SERVO_COUNT] = {
        g_servo_pw_us[PAN_SERVO_IDX],
        g_servo_pw_us[TILT_SERVO_IDX],
    };
    const uint16_t bat_mv = g_battery_mv;
    const bool enc_valid[ENCODER_COUNT] = {g_encoder_valid[ENCODER1_IDX],
                                           g_encoder_valid[ENCODER2_IDX]};

    (void)snprintf(
        frame, sizeof(frame),
        "TEL,RC,%u,%u,%u,%u,%u,%u,ULT,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,"
        "ULT_EN,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,MTR,%u,%u,SRV,%u,%u,BAT,%u,"
        "IMU_OK,%u,IMU,%d,%d,%d,%d,%d,%d,ENC_OK,%u,%u,ENC,%d,%d\n",
        rc_snapshot[0], rc_snapshot[1], rc_snapshot[2], rc_snapshot[3],
        rc_snapshot[4], rc_snapshot[5], ult_snapshot[0], ult_snapshot[1],
        ult_snapshot[2], ult_snapshot[3], ult_snapshot[4], ult_snapshot[5],
        ult_snapshot[6], ult_snapshot[7], ult_snapshot[8], ult_snapshot[9],
        ult_enabled_snapshot[0], ult_enabled_snapshot[1],
        ult_enabled_snapshot[2], ult_enabled_snapshot[3],
        ult_enabled_snapshot[4], ult_enabled_snapshot[5],
        ult_enabled_snapshot[6], ult_enabled_snapshot[7],
        ult_enabled_snapshot[8], ult_enabled_snapshot[9],
        mtr_snapshot[0], mtr_snapshot[1], servo_snapshot[0], servo_snapshot[1],
        bat_mv, g_imu_present ? 1u : 0u, (int)g_imu_accel_mg[0],
        (int)g_imu_accel_mg[1], (int)g_imu_accel_mg[2],
        (int)g_imu_gyro_mdps[0], (int)g_imu_gyro_mdps[1],
        (int)g_imu_gyro_mdps[2], enc_valid[ENCODER1_IDX] ? 1u : 0u,
        enc_valid[ENCODER2_IDX] ? 1u : 0u,
        (int)g_encoder_count[ENCODER1_IDX], (int)g_encoder_count[ENCODER2_IDX]);

    uart_esp32_print(frame);
}

/** Initialize telemetry scheduler state. */
void telemetry_init(void)
{
    g_telem_due = false;
}

/** Mark one telemetry frame as due from the SysTick scheduler. */
void telemetry_schedule(void)
{
    g_telem_due = true;
}

/** Send a scheduled telemetry frame when the service loop reaches telemetry. */
void telemetry_service(void)
{
    if (!g_telem_due)
    {
        return;
    }

    g_telem_due = false;
    telemetry_send_frame();
}
