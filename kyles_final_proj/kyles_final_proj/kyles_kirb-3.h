#ifndef KIRB_H_
#define KIRB_H_

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "ti_msp_dl_config.h"

#define NON_APPLICABLE ((uint32_t)0xFFFFFFFFu)

// GPIO functionality is always a pin function of 0x00000001
#define IOMUX_PINCM_FUNCSEL_GPIO (1u)

// Standard hobby servo pulse widths (microseconds)
#define SERVO_MIN_PULSE_WIDTH_US          (1000u)
#define SERVO_NEUTRAL_PULSE_WIDTH_US      (1500u)
#define SERVO_MAX_PULSE_WIDTH_US          (2000u)

// -----------------------------------------------------------------------------
// RC Channel Indices and Logical Aliases
// -----------------------------------------------------------------------------

typedef enum {
    RC_CH1_IDX,
    RC_CH2_IDX,
    RC_CH3_IDX,
    RC_CH4_IDX,
    RC_CH5_IDX,
    RC_CH6_IDX,
    RC_CH_COUNT,
} RcIndex;

// Logical control mappings (FS-16 style)
#define RC_CH_LS_X   (RC_CH1_IDX)
#define RC_CH_RS_Y   (RC_CH2_IDX)
#define RC_CH_LS_Y   (RC_CH3_IDX)
#define RC_CH_RS_X   (RC_CH4_IDX)
#define RC_CH_VR_A   (RC_CH5_IDX)
#define RC_CH_VR_B   (RC_CH6_IDX)

typedef enum {
    RC_BACKEND_TIMER,
    RC_BACKEND_GPIO,
} RcBackendType;

typedef struct {
    RcBackendType backend;     // TIMER or GPIO-based capture
    GPIO_Regs    *gpio_port;   // Pin bank used for this channel
    uint32_t      gpio_pin;    // Pin mask within that bank
    uint32_t      iomux_pincm; // IOMUX PINCM index for this pin
    uint32_t      iomux_func;  // PF function value for this pin (or NON_APPLICABLE for plain GPIO)
    void         *timer_inst;  // Timer instance used for capture/timebase
    uint32_t      timer_cc;    // CC index for TIMER backend, NON_APPLICABLE for GPIO
    uint32_t      irq_event;   // TIMER: IIDX_CCx_UP, GPIO: pin mask for MIS/IMASK
} RcChannelConfig;

static const RcChannelConfig g_rc_cfg[RC_CH_COUNT] = {

    [RC_CH_LS_X] = {
        .backend    = RC_BACKEND_TIMER,
        .gpio_port  = GPIO_RC_TIM0_C2_PORT,
        .gpio_pin   = GPIO_RC_TIM0_C2_PIN,
        .iomux_pincm = GPIO_RC_TIM0_C2_IOMUX,
        .iomux_func  = GPIO_RC_TIM0_C2_IOMUX_FUNC,
        .timer_inst = RC_TIM0_INST,
        .timer_cc   = DL_TIMER_CC_2_INDEX,
        .irq_event  = DL_TIMER_IIDX_CC2_UP,
    },
    [RC_CH_RS_Y] = {
        .backend    = RC_BACKEND_TIMER,
        .gpio_port  = GPIO_RC_TIM0_C0_PORT,
        .gpio_pin   = GPIO_RC_TIM0_C0_PIN,
        .iomux_pincm = GPIO_RC_TIM0_C0_IOMUX,
        .iomux_func  = GPIO_RC_TIM0_C0_IOMUX_FUNC,
        .timer_inst = RC_TIM0_INST,
        .timer_cc   = DL_TIMER_CC_1_INDEX,
        .irq_event  = DL_TIMER_IIDX_CC1_UP,
    },
    [RC_CH_LS_Y] = {
        .backend    = RC_BACKEND_TIMER,
        .gpio_port  = GPIO_RC_TIM0_C3_PORT,
        .gpio_pin   = GPIO_RC_TIM0_C3_PIN,
        .iomux_pincm = GPIO_RC_TIM0_C3_IOMUX,
        .iomux_func  = GPIO_RC_TIM0_C3_IOMUX_FUNC,
        .timer_inst = RC_TIM0_INST,
        .timer_cc   = DL_TIMER_CC_3_INDEX,
        .irq_event  = DL_TIMER_IIDX_CC3_UP,
    },
    [RC_CH_RS_X] = {
        .backend    = RC_BACKEND_TIMER,
        .gpio_port  = GPIO_RC_TIM1_C0_PORT,
        .gpio_pin   = GPIO_RC_TIM1_C0_PIN,
        .iomux_pincm = GPIO_RC_TIM1_C0_IOMUX,
        .iomux_func  = GPIO_RC_TIM1_C0_IOMUX_FUNC,
        .timer_inst = RC_TIM1_INST,
        .timer_cc   = DL_TIMER_CC_1_INDEX,
        .irq_event  = DL_TIMER_IIDX_CC1_UP,
        // Intentionally shares pin/timebase wiring for PW capture reasons
    },

    [RC_CH_VR_A] = {
        .backend    = RC_BACKEND_GPIO,
        .gpio_port  = RC_IN_PORT,
        .gpio_pin   = RC_IN_CH5_PIN,
        .iomux_pincm = RC_IN_CH5_IOMUX,
        .iomux_func  = IOMUX_PINCM_FUNCSEL_GPIO,
        .timer_inst = RC_TIM0_INST,
        .timer_cc   = NON_APPLICABLE,
        .irq_event  = RC_IN_CH5_PIN,
    },
    [RC_CH_VR_B] = {
        .backend    = RC_BACKEND_GPIO,
        .gpio_port  = RC_IN_PORT,
        .gpio_pin   = RC_IN_CH6_PIN,
        .iomux_pincm = RC_IN_CH6_IOMUX,
        .iomux_func  = IOMUX_PINCM_FUNCSEL_GPIO,
        .timer_inst = RC_TIM0_INST,
        .timer_cc   = NON_APPLICABLE,
        .irq_event  = RC_IN_CH6_PIN,
    },
};

// -----------------------------------------------------------------------------
// Motor Configuration
// -----------------------------------------------------------------------------

typedef enum {
    MOTOR1_IDX,
    MOTOR2_IDX,
    MOTOR_COUNT,
} MtrIndex;

#define L_MTR_IDX   (MOTOR1_IDX)
#define R_MTR_IDX   (MOTOR2_IDX)

typedef struct {
    GPIO_Regs *gpio_port;   // PWM output port
    uint32_t   gpio_pin;    // PWM output pin mask
    uint32_t   iomux_pincm; // IOMUX PINCM index for this PWM pin
    uint32_t   iomux_func;  // PF function value for this PWM pin
    void      *timer_inst;  // PWM timer instance
    uint32_t   timer_cc;    // PWM CC index
    RcIndex    rc_in_ch;    // RC input channel driving this motor
} MtrConfig;

static const MtrConfig g_mtr_cfg[MOTOR_COUNT] = {

    [L_MTR_IDX] = {
        .gpio_port  = GPIO_MOTOR_PWM_C0_PORT,
        .gpio_pin   = GPIO_MOTOR_PWM_C0_PIN,
        .iomux_pincm = GPIO_MOTOR_PWM_C0_IOMUX,
        .iomux_func  = GPIO_MOTOR_PWM_C0_IOMUX_FUNC,
        .timer_inst = MOTOR_PWM_INST,
        .timer_cc   = GPIO_MOTOR_PWM_C0_IDX,
        .rc_in_ch   = RC_CH_LS_Y,
    },
    [R_MTR_IDX] = {
        .gpio_port  = GPIO_MOTOR_PWM_C1_PORT,
        .gpio_pin   = GPIO_MOTOR_PWM_C1_PIN,
        .iomux_pincm = GPIO_MOTOR_PWM_C1_IOMUX,
        .iomux_func  = GPIO_MOTOR_PWM_C1_IOMUX_FUNC,
        .timer_inst = MOTOR_PWM_INST,
        .timer_cc   = GPIO_MOTOR_PWM_C1_IDX,
        .rc_in_ch   = RC_CH_RS_Y,
    },
};

#define L_MTR_RC_IN_CH   (g_mtr_cfg[L_MTR_IDX].rc_in_ch)
#define R_MTR_RC_IN_CH   (g_mtr_cfg[R_MTR_IDX].rc_in_ch)

// -----------------------------------------------------------------------------
// Ultrasonic Distance Sensor Configuration
// -----------------------------------------------------------------------------
#define PING_PIN       (PING_TRIG_PIN)
#define PING_PINCM     (PING_TRIG_IOMUX)
#define PING_TRIG_FUNC (IOMUX_PINCM_FUNCSEL_GPIO)
#define PING_ECHO_FUNC (GPIO_ULT_ECHO_TIM_C0_IOMUX_FUNC)

#define ULT_COUNT      (10)
#define TRIG_PW_US     (5)

#endif // KIRB_H_

