#ifndef MOTOR_H
#define MOTOR_H

#include "rc.h"

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

typedef struct
{
    GPIO_Regs *gpio_port; // PWM output port
    uint32_t gpio_pin;    // PWM output pin mask
    uint32_t iomux_pincm; // IOMUX PINCM index for this PWM pin
    uint32_t iomux_func;  // PF function value for this PWM pin
    void *timer_inst;     // PWM timer instance
    uint32_t timer_cc;    // PWM CC index
    RcIndex rc_in_ch;     // RC input channel driving this motor
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

#endif