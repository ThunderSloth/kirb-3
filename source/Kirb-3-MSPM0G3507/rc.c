#include "rc.h"
#include "kirb-3.h"
#include "ti_msp_dl_config.h"

/** @file rc.c
 *  @brief RC receiver pulse capture for timer-backed and GPIO-backed channels.
 */

volatile uint16_t g_rc_pw_us[RC_CH_COUNT];
static volatile uint16_t g_rc_edge_start_us[RC_CH_COUNT];

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
            // CH5/CH6 arrive through GPIO interrupts, but they are still
            // edge-aligned to the shared 1 MHz RC timer timebase. Sampling the
            // timer count here is therefore still interpreted in microseconds.
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

/** Initialize cached RC pulse widths and GPIO edge-capture timestamps. */
void rc_init(void)
{
    for (RcIndex idx = 0; idx < RC_CH_COUNT; idx++)
    {
        g_rc_pw_us[idx] = RC_PULSE_WIDTH_NEUTRAL_US;
        g_rc_edge_start_us[idx] = 0u;
    }
}

/** Validate and store one channel's measured pulse width. */
static void rc_store_pulse_width(RcIndex chan, uint16_t pulse_width_us)
{
    g_rc_pw_us[chan] = rc_filter_pulse_width_us(pulse_width_us);
}

/** Dispatch RC timer 0 capture events for channels 1, 2, and 3. */
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

    rc_store_pulse_width(
        chan, DL_Timer_getCaptureCompareValue((GPTIMER_Regs *)cfg->timer_inst,
                                              cfg->timer_cc));
}

/** Dispatch RC timer 1 capture events for channel 4. */
void rc_ch4_irq(void)
{
    switch (DL_Timer_getPendingInterrupt(RC_TIM1_INST))
    {
    case g_rc_cfg[RC_CH_RS_X].irq_event:
        rc_store_pulse_width(
            RC_CH_RS_X, DL_Timer_getCaptureCompareValue(
                            (GPTIMER_Regs *)g_rc_cfg[RC_CH_RS_X].timer_inst,
                            g_rc_cfg[RC_CH_RS_X].timer_cc));
        break;
    default:
        return;
    }
}

/** Measure GPIO-backed RC channels 5 and 6 from shared timer timestamps. */
void rc_ch5_6_irq(void)
{
    const uint32_t rc_in_status = DL_GPIO_getEnabledInterruptStatus(
        RC_IN_PORT,
        (g_rc_cfg[RC_CH_VR_A].gpio_pin | g_rc_cfg[RC_CH_VR_B].gpio_pin));
    const uint16_t now_us =
        (uint16_t)DL_Timer_getTimerCount(g_rc_cfg[RC_CH_VR_A].timer_inst);

    if (rc_in_status & g_rc_cfg[RC_CH_VR_A].gpio_pin)
    {
        const bool pin_high =
            (DL_GPIO_readPins(RC_IN_PORT, g_rc_cfg[RC_CH_VR_A].gpio_pin) != 0u);

        if (pin_high)
        {
            g_rc_edge_start_us[RC_CH_VR_A] = now_us;
        }
        else
        {
            const uint16_t pulse_width_us =
                (uint16_t)(now_us - g_rc_edge_start_us[RC_CH_VR_A]);
            rc_store_pulse_width(RC_CH_VR_A, pulse_width_us);
        }

        DL_GPIO_clearInterruptStatus(RC_IN_PORT,
                                     (g_rc_cfg[RC_CH_VR_A].gpio_pin));
    }

    if (rc_in_status & g_rc_cfg[RC_CH_VR_B].gpio_pin)
    {
        const bool pin_high =
            (DL_GPIO_readPins(RC_IN_PORT, g_rc_cfg[RC_CH_VR_B].gpio_pin) != 0u);

        if (pin_high)
        {
            g_rc_edge_start_us[RC_CH_VR_B] = now_us;
        }
        else
        {
            const uint16_t pulse_width_us =
                (uint16_t)(now_us - g_rc_edge_start_us[RC_CH_VR_B]);
            rc_store_pulse_width(RC_CH_VR_B, pulse_width_us);
        }

        DL_GPIO_clearInterruptStatus(RC_IN_PORT,
                                     (g_rc_cfg[RC_CH_VR_B].gpio_pin));
    }
}
