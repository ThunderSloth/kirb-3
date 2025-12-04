#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "kyles_kirb-3_config.h"
#include "kyles_kirb-3.h"

//-----------------------------------------------------------------------------
// Development Notes (not part of compiled logic)
//-----------------------------------------------------------------------------
// Temporary reminders, debugging notes, and reference snippets. This section
// has no effect on program behavior and may be removed at any time.
/*
   Git workflow:
     git pull
     git add .
     git commit -m "message"
     git push

//-----------------------------------------------------------------------------*/


// RC Pulse Width Captures in Microseconds
volatile uint16_t g_rc_pw_us[RC_CH_COUNT];

// Ultrasonic Echo Pulse Width Captures in Microseconds
volatile uint16_t g_ult_pw_us[ULT_COUNT];

// Sensor Index
volatile uint8_t g_ult_idx = 0;

bool g_ignore_systick = true;

int main(void)
{
    for (RcIndex idx = 0; idx < RC_CH_COUNT; idx++) {
        g_rc_pw_us[idx] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    }

    config_init();

    NVIC_EnableIRQ(RC_TIM0_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_TIM1_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_IN_INT_IRQN);
    NVIC_EnableIRQ(ULT_SCHED_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(ULT_ECHO_TIM_INST_INT_IRQN);
 
    while (1) {

        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST , g_rc_pw_us[L_MTR_RC_IN_CH], g_mtr_cfg[L_MTR_IDX].timer_cc);
        DL_Timer_setCaptureCompareValue(MOTOR_PWM_INST , g_rc_pw_us[R_MTR_RC_IN_CH], g_mtr_cfg[R_MTR_IDX].timer_cc);
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

void SysTick_Handler(void)
{
  static uint16_t delay_time = 1;
  
  static bool is_buzzing = false;
  
  if (g_ignore_systick == false){
    delay_time--;
    if (delay_time == 0)
    {
      if (is_buzzing == false)  //If statement for toggling the buzzer
      {
        GPIOB->DOUT31_0 |= GPIO_DOUT31_0_DIO13_MASK;
        is_buzzing = true;      
      }
      else 
      {
        GPIOB->DOUT31_0 &= ~GPIO_DOUT31_0_DIO13_MASK;
        is_buzzing = false;
      }
      
      delay_time = 100;        //May need to adjust delay time when testing
      
    } /*if*/
  }
}


void scale_motor_speed(void)
{
    uint16_t var_res = g_rc_pw_us[RC_CH_VR_A];
    uint16_t mtr_val;

    uint8_t scale_percent = (var_res - 1000)/1000;
//-----------------------------------------
    mtr_val = g_rc_pw_us[L_MTR_RC_IN_CH];

    int16_t mtr_diff = mtr_val - 1500;
    mtr_diff = mtr_diff * scale_percent;

    mtr_val = 1500 + mtr_diff;

    g_rc_pw_us[L_MTR_RC_IN_CH] = mtr_val;
//-----------------------------------------
    mtr_val = g_rc_pw_us[R_MTR_RC_IN_CH];

    mtr_diff = mtr_val - 1500;
    mtr_diff = mtr_diff * scale_percent;

    mtr_val = 1500 + mtr_diff;

    g_rc_pw_us[R_MTR_RC_IN_CH] = mtr_val;


}


void ULT_SCHED_TIM_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(ULT_SCHED_TIM_INST)) {
        case DL_TIMER_INTERRUPT_ZERO_EVENT:

            // Disable Echo Timer
            ULT_SCHED_TIM_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_DISABLED;

            // Disable Ping IO output driver
            PING_PORT->DOECLR31_0 = PING_PIN;
            // // Route Ping IO pin to trigger mode (GPIO output)
            IOMUX->SECCFG.PINCM[PING_PINCM] = PING_TRIG_FUNC;

            // Determine branch and channel for Sensor Index

            // Set Mux channel-select lines

            // Set Buffer direction to output

            // Enable selected Mux branch

            // Set Ping IO output level HIGH

            // Enable Ping IO output driver

            break;
        case DL_TIMER_INTERRUPT_CC0_UP_EVENT:
            // Disable Ping IO output driver

            // Route Ping IO pin to Echo Timer input capture (CC0 & CC1)

            // Set Buffer direction to input

            // Enable Echo Timer

            break;
        case DL_TIMER_INTERRUPT_LOAD_EVENT:
            // Advance Sensor Index
            if (++g_ult_idx >= ULT_COUNT) {
                g_ult_idx = 0;
            }
            break;
        default:
            return;
    }
}

void ULT_ECHO_TIM_INST_IRQHandler(void)
{
    switch (DL_Timer_getPendingInterrupt(ULT_ECHO_TIM_INST)) {
        case DL_TIMER_INTERRUPT_CC1_UP_EVENT:
            // Store CC1 capture as pulse width for current sensor

            // Disable Echo Timer
            break;
        default:
            return;
    }
}

//Need to add to main loop
void check_for_reverse(void)
{
  //Variables to hold the volatile RC data
  uint16_t rs_y_value = g_rc_pw_us[RC_CH_RS_Y];
  uint16_t ls_y_value = g_rc_pw_us[RC_CH_LS_Y];

  if (rs_y_value < SERVO_NEUTRAL_PULSE_WIDTH_US && ls_y_value < SERVO_NEUTRAL_PULSE_WIDTH_US)
  {
    g_ignore_systick = false;
  }
  else 
  {
    g_ignore_systick = true;
  }
}

