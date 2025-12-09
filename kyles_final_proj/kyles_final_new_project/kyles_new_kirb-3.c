#include "kyles_new_kirb-3.h"

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
        uint16_t lmtr_val= g_rc_pw_us[L_MTR_RC_IN_CH];
        uint16_t rmtr_val= g_rc_pw_us[R_MTR_RC_IN_CH];
        uint16_t var_res = g_rc_pw_us[RC_CH_VR_A];
        uint8_t scale_percent = (var_res - SERVO_MIN_PULSE_WIDTH_US)/SERVO_MIN_PULSE_WIDTH_US;
        //------------------------------------------------------------------
        if (lmtr_val - rmtr_val > (-1 * SERVO_MIN_PULSE_WIDTH_DIFF_US) & 
            lmtr_val - rmtr_val < SERVO_MIN_PULSE_WIDTH_DIFF_US)
        {
            lmtr_val = rmtr_val;
        }

        //--------------------------------------------------------------
        int16_t mtr_diff = lmtr_val - SERVO_NEUTRAL_PULSE_WIDTH_US;
        mtr_diff = mtr_diff * scale_percent;

        lmtr_val = SERVO_NEUTRAL_PULSE_WIDTH_US + mtr_diff;

        //---------------------------------------------------------------
        mtr_diff = rmtr_val - SERVO_NEUTRAL_PULSE_WIDTH_US;
        mtr_diff = mtr_diff * scale_percent;

        rmtr_val = SERVO_NEUTRAL_PULSE_WIDTH_US + mtr_diff;

        //-------------------------------------------------------------
        void scale_motor_speed(void);
        void set_drive_straight(void);
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST,
                                        lmtr_val,
                                        g_mtr_cfg[L_MTR_IDX].timer_cc);
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST,
                                        rmtr_val,
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
/*
//-----------------------------------------------------------------------------
// scale_motor_speed
//-----------------------------------------------------------------------------
// Scales the motor speed based on variable resistor input and pulse width
// data from remote control
//-----------------------------------------------------------------------------
void scale_motor_speed(void)
{
    uint16_t var_res = g_rc_pw_us[RC_CH_VR_A];
    uint16_t mtr_val;

    uint8_t scale_percent = (var_res - SERVO_MIN_PULSE_WIDTH_US)/SERVO_MIN_PULSE_WIDTH_US;
//-----------------------------------------
    mtr_val = g_rc_pw_us[L_MTR_RC_IN_CH];

    int16_t mtr_diff = mtr_val - SERVO_NEUTRAL_PULSE_WIDTH_US;
    mtr_diff = mtr_diff * scale_percent;

    mtr_val = SERVO_NEUTRAL_PULSE_WIDTH_US + mtr_diff;

    g_rc_pw_us[L_MTR_RC_IN_CH] = mtr_val;
//-----------------------------------------
    mtr_val = g_rc_pw_us[R_MTR_RC_IN_CH];

    mtr_diff = mtr_val - SERVO_NEUTRAL_PULSE_WIDTH_US;
    mtr_diff = mtr_diff * scale_percent;

    mtr_val = SERVO_NEUTRAL_PULSE_WIDTH_US + mtr_diff;

    g_rc_pw_us[R_MTR_RC_IN_CH] = mtr_val;
}


//-----------------------------------------------------------------------------
// set_drive_straight
//-----------------------------------------------------------------------------
// Checks if motor values are within a sertain range of each other and sets 
// the Left value to the Right if they are This will make it easier to drive 
// in a straight line
//-----------------------------------------------------------------------------
void set_drive_straight(void)
{
    uint16_t rmtr_val= L_MTR_RC_IN_CH;
    uint16_t lmtr_val= R_MTR_RC_IN_CH;
    if (lmtr_val - rmtr_val > (-1 * SERVO_MIN_PULSE_WIDTH_DIFF_US) & 
        lmtr_val - rmtr_val < SERVO_MIN_PULSE_WIDTH_DIFF_US)
    {
        g_rc_pw_us[L_MTR_RC_IN_CH] = rmtr_val;
    }
}

*/