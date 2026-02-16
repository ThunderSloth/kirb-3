#include "rc.h"
#include "kyles_new_kirb-3.h"
#include "ti_msp_dl_config.h"

// RC Pulse Width Captures in Microseconds
volatile uint16_t g_rc_pw_us[RC_CH_COUNT];

const RcChannelConfig g_rc_cfg[RC_CH_COUNT] = {

    [RC_CH_LS_X] =
        {
            .backend = RC_BACKEND_TIMER,
            .gpio_port = GPIO_RC_TIM0_C2_PORT,
            .gpio_pin = GPIO_RC_TIM0_C2_PIN,
            .iomux_pincm = GPIO_RC_TIM0_C2_IOMUX,
            .iomux_func = GPIO_RC_TIM0_C2_IOMUX_FUNC,
            .timer_inst = RC_TIM0_INST,
            .timer_cc = DL_TIMER_CC_2_INDEX,
            .irq_event = DL_TIMER_IIDX_CC2_UP,
        },
    [RC_CH_RS_Y] =
        {
            .backend = RC_BACKEND_TIMER,
            .gpio_port = GPIO_RC_TIM0_C0_PORT,
            .gpio_pin = GPIO_RC_TIM0_C0_PIN,
            .iomux_pincm = GPIO_RC_TIM0_C0_IOMUX,
            .iomux_func = GPIO_RC_TIM0_C0_IOMUX_FUNC,
            .timer_inst = RC_TIM0_INST,
            .timer_cc = DL_TIMER_CC_1_INDEX,
            .irq_event = DL_TIMER_IIDX_CC1_UP,
        },
    [RC_CH_LS_Y] =
        {
            .backend = RC_BACKEND_TIMER,
            .gpio_port = GPIO_RC_TIM0_C3_PORT,
            .gpio_pin = GPIO_RC_TIM0_C3_PIN,
            .iomux_pincm = GPIO_RC_TIM0_C3_IOMUX,
            .iomux_func = GPIO_RC_TIM0_C3_IOMUX_FUNC,
            .timer_inst = RC_TIM0_INST,
            .timer_cc = DL_TIMER_CC_3_INDEX,
            .irq_event = DL_TIMER_IIDX_CC3_UP,
        },
    [RC_CH_RS_X] =
        {
            .backend = RC_BACKEND_TIMER,
            .gpio_port = GPIO_RC_TIM1_C0_PORT,
            .gpio_pin = GPIO_RC_TIM1_C0_PIN,
            .iomux_pincm = GPIO_RC_TIM1_C0_IOMUX,
            .iomux_func = GPIO_RC_TIM1_C0_IOMUX_FUNC,
            .timer_inst = RC_TIM1_INST,
            .timer_cc = DL_TIMER_CC_1_INDEX,
            .irq_event = DL_TIMER_IIDX_CC1_UP,
            // Intentionally shares pin/timebase wiring for PW capture reasons
        },

    [RC_CH_VR_A] =
        {
            .backend = RC_BACKEND_GPIO,
            .gpio_port = RC_IN_PORT,
            .gpio_pin = RC_IN_CH5_PIN,
            .iomux_pincm = RC_IN_CH5_IOMUX,
            .iomux_func = IOMUX_PINCM_FUNCSEL_GPIO,
            .timer_inst = RC_TIM0_INST,
            .timer_cc = NON_APPLICABLE,
            .irq_event = RC_IN_CH5_PIN,
        },
    [RC_CH_VR_B] =
        {
            .backend = RC_BACKEND_GPIO,
            .gpio_port = RC_IN_PORT,
            .gpio_pin = RC_IN_CH6_PIN,
            .iomux_pincm = RC_IN_CH6_IOMUX,
            .iomux_func = IOMUX_PINCM_FUNCSEL_GPIO,
            .timer_inst = RC_TIM0_INST,
            .timer_cc = NON_APPLICABLE,
            .irq_event = RC_IN_CH6_PIN,
        },
};

void rc_ch1_3_irq(void)
{
    RcIndex chan;
    switch (DL_Timer_getPendingInterrupt(RC_TIM0_INST))
    {
    case g_rc_cfg[RC_CH_LS_X].irq_event:
        chan = RC_CH_LS_X;
        break;
    case g_rc_cfg[RC_CH_RS_Y].irq_event:
        chan = RC_CH_RS_Y;
        break;
    case g_rc_cfg[RC_CH_LS_Y].irq_event:
        chan = RC_CH_LS_Y;
        break;
    default:
        return;
    }
    const RcChannelConfig *cfg = &g_rc_cfg[chan];

    g_rc_pw_us[chan] = DL_Timer_getCaptureCompareValue(
        (GPTIMER_Regs *)cfg->timer_inst, cfg->timer_cc);
}

void rc_ch4_irq(void)
{
    switch (DL_Timer_getPendingInterrupt(RC_TIM1_INST))
    {
    case g_rc_cfg[RC_CH_RS_X].irq_event:
        g_rc_pw_us[RC_CH_RS_X] = DL_Timer_getCaptureCompareValue(
            (GPTIMER_Regs *)g_rc_cfg[RC_CH_RS_X].timer_inst,
            g_rc_cfg[RC_CH_RS_X].timer_cc);
        break;
    default:
        return;
    }
}

void rc_ch5_6_irq(void)
{
    const uint32_t rc_in_status = DL_GPIO_getEnabledInterruptStatus(
        RC_IN_PORT,
        (g_rc_cfg[RC_CH_VR_A].gpio_pin | g_rc_cfg[RC_CH_VR_B].gpio_pin));

    if (rc_in_status & g_rc_cfg[RC_CH_VR_A].gpio_pin)
    {
        g_rc_pw_us[RC_CH_VR_A] =
            DL_Timer_getTimerCount(g_rc_cfg[RC_CH_VR_A].timer_inst);
        DL_GPIO_clearInterruptStatus(RC_IN_PORT,
                                     (g_rc_cfg[RC_CH_VR_A].gpio_pin));
    }
    if (rc_in_status & g_rc_cfg[RC_CH_VR_B].gpio_pin)
    {
        g_rc_pw_us[RC_CH_VR_B] =
            DL_Timer_getTimerCount(g_rc_cfg[RC_CH_VR_B].timer_inst);
        DL_GPIO_clearInterruptStatus(RC_IN_PORT,
                                     (g_rc_cfg[RC_CH_VR_B].gpio_pin));
    }
}