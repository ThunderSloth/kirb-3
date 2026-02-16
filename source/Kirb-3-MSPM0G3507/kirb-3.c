#include "kirb-3.h"

#include "cmd_shell.h"
#include "encoder.h"
#include "motor.h"
#include "ping.h"
#include "rc.h"


// RC Pulse Width Captures in Microseconds
volatile uint16_t g_rc_pw_us[RC_CH_COUNT];

int main(void)
{
    for (RcIndex idx = 0; idx < RC_CH_COUNT; idx++) {
        g_rc_pw_us[idx] = SERVO_NUETRAL_PULSE_WIDTH_US;
    }

    SYSCFG_DL_init();
    ping_init();
    encoder_init();

    NVIC_EnableIRQ(RC_TIM0_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_TIM1_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_IN_INT_IRQN);
    NVIC_EnableIRQ(ULT_SCHED_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(ULT_ECHO_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_ENCODER_INST_INT_IRQN);

    arm_encoder_dma();

    cmd_shell_print_boot_msg();

    // uart tx testing
    uart_encoder_query(0xA, 0);
    uart_encoder_query(0xC, 0);

    while (1)
    {
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST,
                                        g_rc_pw_us[L_MTR_RC_IN_CH],
                                        g_mtr_cfg[L_MTR_IDX].timer_cc);
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST,
                                        g_rc_pw_us[R_MTR_RC_IN_CH],
                                        g_mtr_cfg[R_MTR_IDX].timer_cc);

        cmd_shell_poll();
 
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

void ULT_SCHED_TIM_INST_IRQHandler(void)
{
    ult_sched_irq();
}

void ULT_ECHO_TIM_INST_IRQHandler(void)
{
    ult_echo_irq();
}

void UART_ENCODER_INST_IRQHandler(void)
{
    uart_encoder_irq();
}
