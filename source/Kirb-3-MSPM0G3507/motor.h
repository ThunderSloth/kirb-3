#ifndef MOTOR_H
#define MOTOR_H

/** @file motor.h
 *  @brief Motor and pan/tilt servo PWM helpers driven from RC inputs.
 */

#include "rc.h"
#include "system_state.h"

/** Standard hobby servo pulse widths, in microseconds. */
#define SERVO_MIN_PULSE_WIDTH_US (1000u)
#define SERVO_NEUTRAL_PULSE_WIDTH_US (1500u)
#define SERVO_MAX_PULSE_WIDTH_US (2000u)
#define SERVO_MIN_PULSE_WIDTH_DIFF_US (50u)

// -----------------------------------------------------------------------------
// Motor Configuration
// -----------------------------------------------------------------------------

typedef enum
{
    MOTOR1_IDX,
    MOTOR2_IDX,
    MOTOR_COUNT,
} MtrIndex;

#define L_MTR_IDX (MOTOR1_IDX)
#define R_MTR_IDX (MOTOR2_IDX)

/** Cached motor output pulse widths, in microseconds. */
extern volatile uint16_t g_mtr_pw_us[MOTOR_COUNT];

/** Initialize motor and servo outputs to neutral pulse widths. */
void motor_init(void);

/** Hardware and RC-channel mapping for one PWM output. */
typedef struct
{
    GPIO_Regs *gpio_port;
    uint32_t gpio_pin;
    uint32_t iomux_pincm;
    uint32_t iomux_func;
    void *timer_inst;
    uint32_t timer_cc;
    RcIndex rc_in_ch;
} MtrConfig;

static const MtrConfig g_mtr_cfg[MOTOR_COUNT] = {

    [L_MTR_IDX] =
        {
            .gpio_port = GPIO_MOTOR_PWM_C0_PORT,
            .gpio_pin = GPIO_MOTOR_PWM_C0_PIN,
            .iomux_pincm = GPIO_MOTOR_PWM_C0_IOMUX,
            .iomux_func = GPIO_MOTOR_PWM_C0_IOMUX_FUNC,
            .timer_inst = MOTOR_PWM_INST,
            .timer_cc = GPIO_MOTOR_PWM_C0_IDX,
            .rc_in_ch = RC_CH_LS_Y,
        },
    [R_MTR_IDX] =
        {
            .gpio_port = GPIO_MOTOR_PWM_C1_PORT,
            .gpio_pin = GPIO_MOTOR_PWM_C1_PIN,
            .iomux_pincm = GPIO_MOTOR_PWM_C1_IOMUX,
            .iomux_func = GPIO_MOTOR_PWM_C1_IOMUX_FUNC,
            .timer_inst = MOTOR_PWM_INST,
            .timer_cc = GPIO_MOTOR_PWM_C1_IDX,
            .rc_in_ch = RC_CH_RS_Y,
        },
};

#define L_MTR_RC_IN_CH (g_mtr_cfg[L_MTR_IDX].rc_in_ch)
#define R_MTR_RC_IN_CH (g_mtr_cfg[R_MTR_IDX].rc_in_ch)

// -----------------------------------------------------------------------------
// Camera Pan-Tilt Servo Configuration
// -----------------------------------------------------------------------------

typedef enum
{
    SERVO1_IDX,
    SERVO2_IDX,
    SERVO_COUNT,
} ServoIndex;

#define PAN_SERVO_IDX (SERVO1_IDX)
#define TILT_SERVO_IDX (SERVO2_IDX)

/** Cached camera servo output pulse widths, in microseconds. */
extern volatile uint16_t g_servo_pw_us[SERVO_COUNT];

static const MtrConfig g_servo_cfg[SERVO_COUNT] = {

    [PAN_SERVO_IDX] =
        {
            .gpio_port = GPIO_SERVO_PWM_C0_PORT,
            .gpio_pin = GPIO_SERVO_PWM_C0_PIN,
            .iomux_pincm = GPIO_SERVO_PWM_C0_IOMUX,
            .iomux_func = GPIO_SERVO_PWM_C0_IOMUX_FUNC,
            .timer_inst = SERVO_PWM_INST,
            .timer_cc = GPIO_SERVO_PWM_C0_IDX,
            .rc_in_ch = RC_CH_VR_A,
        },
    [TILT_SERVO_IDX] =
        {
            .gpio_port = GPIO_SERVO_PWM_C1_PORT,
            .gpio_pin = GPIO_SERVO_PWM_C1_PIN,
            .iomux_pincm = GPIO_SERVO_PWM_C1_IOMUX,
            .iomux_func = GPIO_SERVO_PWM_C1_IOMUX_FUNC,
            .timer_inst = SERVO_PWM_INST,
            .timer_cc = GPIO_SERVO_PWM_C1_IDX,
            .rc_in_ch = RC_CH_VR_B,
        },
};

#define PAN_SERVO_RC_IN_CH (g_servo_cfg[PAN_SERVO_IDX].rc_in_ch)
#define TILT_SERVO_RC_IN_CH (g_servo_cfg[TILT_SERVO_IDX].rc_in_ch)

/** Apply one PWM pulse width and mirror it into the matching cache. */
static inline void set_pwm_pulse_width(GPTIMER_Regs *const timer_inst,
                                       uint32_t timer_cc,
                                       volatile uint16_t *output_pw_us,
                                       uint32_t output_idx,
                                       uint16_t pulse_width_us)
{
    DL_Timer_setCaptureCompareValue(timer_inst, pulse_width_us, timer_cc);
    output_pw_us[output_idx] = pulse_width_us;
}

/** Filter an RC channel and apply it to one PWM output. */
static inline void update_pwm_from_rc(uint32_t rc_ch,
                                      GPTIMER_Regs *const timer_inst,
                                      uint32_t timer_cc,
                                      volatile uint16_t *output_pw_us,
                                      uint32_t output_idx)
{
    const uint16_t pulse_width_us = rc_filter_pulse_width_us(g_rc_pw_us[rc_ch]);

    set_pwm_pulse_width(timer_inst, timer_cc, output_pw_us, output_idx,
                        pulse_width_us);
}

/** Update left and right drive motors, forcing neutral when drive is disabled. */
static inline void update_motor()
{
    if (!system_state_drive_enabled())
    {
        set_pwm_pulse_width(MOTOR_PWM_INST, g_mtr_cfg[L_MTR_IDX].timer_cc,
                            g_mtr_pw_us, L_MTR_IDX,
                            SERVO_NEUTRAL_PULSE_WIDTH_US);
        set_pwm_pulse_width(MOTOR_PWM_INST, g_mtr_cfg[R_MTR_IDX].timer_cc,
                            g_mtr_pw_us, R_MTR_IDX,
                            SERVO_NEUTRAL_PULSE_WIDTH_US);
        return;
    }

    update_pwm_from_rc(L_MTR_RC_IN_CH, MOTOR_PWM_INST,
                       g_mtr_cfg[L_MTR_IDX].timer_cc, g_mtr_pw_us, L_MTR_IDX);
    update_pwm_from_rc(R_MTR_RC_IN_CH, MOTOR_PWM_INST,
                       g_mtr_cfg[R_MTR_IDX].timer_cc, g_mtr_pw_us, R_MTR_IDX);
}

/** Update the camera pan and tilt servos from their RC channels. */
static inline void update_servo()
{
    update_pwm_from_rc(PAN_SERVO_RC_IN_CH, SERVO_PWM_INST,
                       g_servo_cfg[PAN_SERVO_IDX].timer_cc, g_servo_pw_us,
                       PAN_SERVO_IDX);
    update_pwm_from_rc(TILT_SERVO_RC_IN_CH, SERVO_PWM_INST,
                       g_servo_cfg[TILT_SERVO_IDX].timer_cc, g_servo_pw_us,
                       TILT_SERVO_IDX);
}

#endif // MOTOR_H
