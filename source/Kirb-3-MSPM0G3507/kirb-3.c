#include "kirb-3.h"

#include "cmd_shell.h"
#include "motor.h"
#include "ping.h"
#include "rc.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "uart_debug.h"

int main(void)
{
    for (RcIndex idx = 0; idx < RC_CH_COUNT; idx++)
    {
        g_rc_pw_us[idx] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    }

    for (UltIndex idx = 0; idx < ULT_COUNT; idx++)
    {
        g_ult_pw_us[idx] = ULT_INIT_PW;
    }

    SYSCFG_DL_init();
    ping_init();

    NVIC_EnableIRQ(RC_TIM0_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_TIM1_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_IN_INT_IRQN);
    NVIC_EnableIRQ(ULT_SCHED_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(ULT_ECHO_TIM_INST_INT_IRQN);

    cmd_shell_print_boot_msg();

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

void RC_TIM0_INST_IRQHandler(void)
{
    rc_ch1_3_irq();
}

void RC_TIM1_INST_IRQHandler(void)
{
    rc_ch4_irq();
}

void GROUP1_IRQHandler(void)
{
    rc_ch5_6_irq();
}

void ULT_SCHED_TIM_INST_IRQHandler(void)
{
    ult_sched_irq();
}

void ULT_ECHO_TIM_INST_IRQHandler(void)
{
    ult_echo_irq();
}
