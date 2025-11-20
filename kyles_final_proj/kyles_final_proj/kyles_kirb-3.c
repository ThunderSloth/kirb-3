#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "kyles_kirb-3.h"


// RC Pulse Width Captures in Microseconds
volatile uint16_t g_rc_pw_us[RC_CH_COUNT];

int main(void)
{
    for (RcIndex idx = 0; idx < RC_CH_COUNT; idx++) {
        g_rc_pw_us[idx] = SERVO_NUETRAL_PULSE_WIDTH_US;
    }

    SYSCFG_DL_init();

    NVIC_EnableIRQ(RC_TIM0_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_TIM1_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_IN_INT_IRQN);
 
    while (1) {
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST , g_rc_pw_us[RC_CH_LS_Y], MOTOR1_CC_IDX);
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST , g_rc_pw_us[RC_CH_RS_Y], MOTOR2_CC_IDX);
        __NOP();
    }
}

void RC_TIM0_INST_IRQHandler (void)
{
    RcIndex chan;
    switch (DL_Timer_getPendingInterrupt(RC_TIM0_INST)) {
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
        (GPTIMER_Regs *)cfg->timer_inst,
        cfg->timer_cc
    );
}

void RC_TIM1_INST_IRQHandler (void)
{
    switch (DL_Timer_getPendingInterrupt(RC_TIM1_INST)) {
        case g_rc_cfg[RC_CH_RS_X].irq_event:
            g_rc_pw_us[RC_CH_RS_X] = DL_Timer_getCaptureCompareValue(
                (GPTIMER_Regs *)g_rc_cfg[RC_CH_RS_X].timer_inst,
                 g_rc_cfg[RC_CH_RS_X].timer_cc);
            break;
        default:
            return;
    }
}

void GROUP1_IRQHandler(void)
{
    const uint32_t rc_in_status = DL_GPIO_getEnabledInterruptStatus(
        RC_IN_PORT, (g_rc_cfg[RC_CH_VR_A].gpio_pin | g_rc_cfg[RC_CH_VR_B].gpio_pin));

    if (rc_in_status & g_rc_cfg[RC_CH_VR_A].gpio_pin) {
        g_rc_pw_us[RC_CH_VR_A] = DL_Timer_getTimerCount(g_rc_cfg[RC_CH_VR_A].timer_inst);
        DL_GPIO_clearInterruptStatus(RC_IN_PORT, (g_rc_cfg[RC_CH_VR_A].gpio_pin));
    } 
    if (rc_in_status & g_rc_cfg[RC_CH_VR_B].gpio_pin) {
        g_rc_pw_us[RC_CH_VR_B] = DL_Timer_getTimerCount(g_rc_cfg[RC_CH_VR_B].timer_inst);
        DL_GPIO_clearInterruptStatus(RC_IN_PORT, (g_rc_cfg[RC_CH_VR_B].gpio_pin));
    }
}



