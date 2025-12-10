#include "kyles_new_kirb-3.h"

#include <stdio.h>
#include <inttypes.h>

#include <stdlib.h>
#include "cmd_shell.h"
#include "motor.h"
#include "ping.h"
#include "rc.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "uart_debug.h"
#include "LaunchPad.h"
#include "clock.h"

bool g_disable_buzzer = true;

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
    buzz_init();
    ping_init();
    sys_tick_init(SYS_TICK_PERIOD_COUNT);
    NVIC_EnableIRQ(RC_TIM0_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_TIM1_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_IN_INT_IRQN);
    NVIC_EnableIRQ(ULT_SCHED_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(ULT_ECHO_TIM_INST_INT_IRQN);
  
    cmd_shell_print_boot_msg();

    while (1)
    {   
        uint32_t lmtr_val = g_rc_pw_us[L_MTR_RC_IN_CH];
        uint32_t rmtr_val = g_rc_pw_us[R_MTR_RC_IN_CH];

        lmtr_val = set_drive_straight();  
        check_for_reverse();    
        /*
        float fult_cm = ping_us_to_cm_float(g_ult_pw_us[ANT_CTR_ULT_IDX]);
        float rult_cm = ping_us_to_cm_float(g_ult_pw_us[POST_CTR_ULT_IDX]);

        if (fult_cm < 5 || rult_cm < 5)
        {
            lmtr_val = 1500;
            rmtr_val = 1500;
        }
        */
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

//-----------------------------------------------------------------------------
// SysTick_Handler
//-----------------------------------------------------------------------------
// SysTick Handler for toggling the backup buzzer(andlighting the LED). 
// The buzzer toggling will only occur when the g_disable_buzzer flag is false 
//-----------------------------------------------------------------------------
void SysTick_Handler(void)
{
  static uint16_t delay_time = 1;
  
  static bool is_buzzing = false;
  
  if (g_disable_buzzer == false){
    delay_time--;
    if (delay_time == 0)
    {
      if (is_buzzing == false)  //If statement for toggling the buzzer
      {
        GPIOB->DOUT31_0 |= GPIO_DOUT31_0_DIO9_MASK;
        is_buzzing = true;      
      }
      else 
      {
        GPIOB->DOUT31_0 &= ~GPIO_DOUT31_0_DIO9_MASK;
        is_buzzing = false;
      }
      
      delay_time = 1000;        //May need to adjust delay time when testing
      
    } 
  }
}

void check_for_reverse(void) 
{
  //Variables to hold the volatile RC data
  uint16_t rs_y_value = g_rc_pw_us[RC_CH_RS_Y];
  uint16_t ls_y_value = g_rc_pw_us[RC_CH_LS_Y];

  if (rs_y_value < SERVO_NEUTRAL_PULSE_WIDTH_US && ls_y_value < SERVO_NEUTRAL_PULSE_WIDTH_US)
  {
    g_disable_buzzer = false;
  }
  else 
  {
    //Force output low when not reversing
    GPIOB->DOECLR31_0 = SENS_BUZ_PIN;
    g_disable_buzzer = true;
  }
}

void buzz_init(void)
{
  //Configure IOMUX function and enable output for PB13 (Buzzer) 
  IOMUX->SECCFG.PINCM[SENS_BUZ_IOMUX] = IOMUX_PINCM_PC_CONNECTED |
                                        IOMUX_PINCM26_PF_GPIOB_DIO09;

  GPIOB->DOE31_0 |= GPIO_DOE31_0_DIO9_ENABLE;
}

//-----------------------------------------------------------------------------
// set_drive_straight
//-----------------------------------------------------------------------------
// Checks if motor values are within a sertain range of each other and sets 
// the Left value to the Right if they are This will make it easier to drive 
// in a straight line
//-----------------------------------------------------------------------------
uint32_t set_drive_straight(void)
{
  uint16_t lmtr_val= g_rc_pw_us[L_MTR_RC_IN_CH];
  uint16_t rmtr_val= g_rc_pw_us[R_MTR_RC_IN_CH];

  //==============================================================
  if (abs(lmtr_val - rmtr_val) <  SERVO_MIN_PULSE_WIDTH_DIFF_US)
  {
    lmtr_val = rmtr_val;
  }

  return lmtr_val;
}
//uint16_t var_res = g_rc_pw_us[RC_CH_VR_A];
//uint16_t scale_percent;
//scale_percent = (var_res - SERVO_MIN_PULSE_WIDTH_US)/SERVO_MIN_PULSE_WIDTH_US;
