#ifndef __KIRB_H__
#define __KIRB_H__

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>
#include "ti_msp_dl_config.h"

#define NON_APPLICABLE (0)

#define MOTOR1_CC_IDX (DL_TIMER_CC_0_INDEX)
#define MOTOR2_CC_IDX (DL_TIMER_CC_1_INDEX)

#define SERVO_MIN_PULSE_WIDTH_US          (1000)
#define SERVO_NUETRAL_PULSE_WIDTH_US      (1500)
#define SERVO_MAX_PULSE_WIDTH_US          (2000)

typedef enum {
    RC_CH1_IDX,
    RC_CH2_IDX,
    RC_CH3_IDX,
    RC_CH4_IDX,
    RC_CH5_IDX,
    RC_CH6_IDX,
    RC_CH_COUNT
,
} RcIndex;

#define RC_CH_LS_X (RC_CH1_IDX)
#define RC_CH_RS_Y (RC_CH2_IDX)
#define RC_CH_LS_Y (RC_CH3_IDX)
#define RC_CH_RS_X (RC_CH4_IDX)
#define RC_CH_VR_A (RC_CH5_IDX)
#define RC_CH_VR_B (RC_CH6_IDX)

typedef enum {
    RC_BACKEND_TIMER,
    RC_BACKEND_GPIO,
} RcBackendType;

typedef struct {
    RcBackendType backend;
    GPIO_Regs *gpio_port;
    uint32_t gpio_pin;
    void *timer_inst;
    uint32_t timer_cc;
    uint32_t irq_event;
} RcChannelConfig;

static const RcChannelConfig g_rc_cfg[RC_CH_COUNT] = {

    [RC_CH_LS_X] = {
        .backend   = RC_BACKEND_TIMER,
        .gpio_port = GPIO_RC_TIM0_C2_PORT,
        .gpio_pin  = GPIO_RC_TIM0_C2_PIN,
        .timer_inst = RC_TIM0_INST,
        .timer_cc  = DL_TIMER_CC_2_INDEX,
        .irq_event = DL_TIMER_IIDX_CC2_UP,
    },
    [RC_CH_RS_Y] = {
        .backend   = RC_BACKEND_TIMER,
        .gpio_port = GPIO_RC_TIM0_C0_PORT,
        .gpio_pin  = GPIO_RC_TIM0_C0_PIN,
        .timer_inst = RC_TIM0_INST,
        .timer_cc  = DL_TIMER_CC_1_INDEX,
        .irq_event = DL_TIMER_IIDX_CC1_UP,
    },
    [RC_CH_LS_Y] = {
        .backend   = RC_BACKEND_TIMER,
        .gpio_port = GPIO_RC_TIM0_C3_PORT,
        .gpio_pin  = GPIO_RC_TIM0_C3_PIN,
        .timer_inst = RC_TIM0_INST,
        .timer_cc  = DL_TIMER_CC_3_INDEX,
        .irq_event = DL_TIMER_IIDX_CC3_UP,
    },
    [RC_CH_RS_X] = {
        .backend   = RC_BACKEND_TIMER,
        .gpio_port = GPIO_RC_TIM1_C0_PORT,
        .gpio_pin  = GPIO_RC_TIM1_C0_PIN,
        .timer_inst = RC_TIM1_INST ,
        .timer_cc  = DL_TIMER_CC_1_INDEX,
        .irq_event = DL_TIMER_IIDX_CC1_UP,
    },
    [RC_CH_VR_A] = {
        .backend   = RC_BACKEND_GPIO,
        .gpio_port = RC_IN_PORT,
        .gpio_pin  = RC_IN_CH5_PIN,
        .timer_inst = RC_TIM0_INST,
        .timer_cc  = NON_APPLICABLE,
        .irq_event = RC_IN_CH5_PIN,
    },
    [RC_CH_VR_B] = {
        .backend   = RC_BACKEND_GPIO,
        .gpio_port = RC_IN_PORT,
        .gpio_pin  = RC_IN_CH6_PIN,
        .timer_inst = RC_TIM0_INST,
        .timer_cc  = NON_APPLICABLE,
        .irq_event = RC_IN_CH6_PIN,
    },
};


#endif
