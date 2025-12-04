#include "clock.h"
#include "ti/devices/msp/peripherals/hw_gpio.h"
#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "kyles_kirb-3_config.h"
#include "uart.h"
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
 


    uint32_t count = 0;
    while (1) {
        set_drive_straight();
        scale_motor_speed();
        setCaptureCompareValue(MOTOR_PWM_INST, g_rc_pw_us[L_MTR_RC_IN_CH], g_mtr_cfg[L_MTR_IDX].timer_cc);
        setCaptureCompareValue(MOTOR_PWM_INST, g_rc_pw_us[R_MTR_RC_IN_CH], g_mtr_cfg[R_MTR_IDX].timer_cc);
        __NOP();
        msec_delay(10);
        if (count++ <= 10000){
            count = 0;
            for (uint32_t i = 0; i < ULT_COUNT; i++) {
                msp_printf("%u. ", i);
                msp_printf("%u\n\r", g_ult_pw_us[i]);
            }
        }
    }
}

void RC_TIM0_INST_IRQHandler (void)
{
    RcIndex chan;
    switch (getPendingInterrupt(RC_TIM0_INST)) {
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

    g_rc_pw_us[chan] = getCaptureCompareValue(
        (GPTIMER_Regs *)cfg->timer_inst,
        cfg->timer_cc
    );
}

void RC_TIM1_INST_IRQHandler (void)
{
    switch (getPendingInterrupt(RC_TIM1_INST)) {
        case g_rc_cfg[RC_CH_RS_X].irq_event:
            g_rc_pw_us[RC_CH_RS_X] = getCaptureCompareValue(
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

//Scales the motor speed based on variable resistor input from remote controll
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


// Checks if motor values are within a sertain range of each other and sets the Left value to the Right if they are
// This will make it easier to drive in a straight line
void set_drive_straight(void)
{
    if (g_rc_pw_us[L_MTR_RC_IN_CH] - g_rc_pw_us[R_MTR_RC_IN_CH] > (-1 * SERVO_MIN_PULSE_WIDTH_DIFF_US) & 
        g_rc_pw_us[L_MTR_RC_IN_CH] - g_rc_pw_us[R_MTR_RC_IN_CH] < SERVO_MIN_PULSE_WIDTH_DIFF_US)
    {
        g_rc_pw_us[L_MTR_RC_IN_CH] = g_rc_pw_us[R_MTR_RC_IN_CH];
    }
}


void ULT_SCHED_TIM_INST_IRQHandler(void)
{
    switch (getPendingInterrupt(ULT_SCHED_TIM_INST)) {
        case DL_TIMER_IIDX_ZERO: 
        {
            // Disable Echo Timer
            ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);

            // Disable Ping IO output driver
            PING_PORT->DOECLR31_0 = PING_PIN;
            // Route Ping IO pin to trigger mode (GPIO output)
            IOMUX->SECCFG.PINCM[PING_PINCM] = PING_TRIG_FUNC;

            // Disable both Mux
            for (uint8_t mux_idx = 0; mux_idx < MUX_COUNT; mux_idx++) {
                
                const MuxEnConfig *cfg = &g_mux_en_cfg[mux_idx];

                GPIO_Regs *port = cfg->gpio_port;
                uint32_t   pin  = cfg->gpio_pin;

                port->DOUTCLR31_0 = pin;
            }

            // Determine branch and channel for Sensor Index
            uint8_t branch = g_ult_idx % ULTS_PER_MUX;
            uint8_t channel = g_ult_idx / ULTS_PER_MUX;

            // Set Mux channel-select lines
            for (uint8_t sel_idx = 0; sel_idx < MUX_SEL_COUNT; sel_idx++) {

                const MuxSelConfig *cfg = &g_mux_sel_cfg[sel_idx];

                GPIO_Regs *port = cfg->gpio_port;
                uint32_t   pin  = cfg->gpio_pin;

                bool bit = (channel >> sel_idx) & 1;

                if (bit) {
                    port->DOUTSET31_0 = pin;
                } else {
                    port->DOUTCLR31_0 = pin;
                }
            }

            // Set Buffer direction to output
            BUF_PORT->DOUTSET31_0 = BUF_DIR_PIN;

            // Enable selected Mux branch
            const MuxEnConfig *mux = &g_mux_en_cfg[branch];

            GPIO_Regs *mux_port = mux->gpio_port;
            uint32_t   mux_pin  = mux->gpio_pin;

            mux_port->DOUTSET31_0 = mux_pin;

            // Set Ping IO output level HIGH
            PING_PORT->DOUTSET31_0 = PING_PIN;
            // Enable Ping IO output driver
            PING_PORT->DOESET31_0 = PING_PIN;

            break;
        }
        case DL_TIMER_IIDX_CC0_UP:
        {
            // Set PING IO Output Level LOW
            PING_PORT->DOUTCLR31_0 = PING_PIN;
            // Disable Ping IO output driver
            PING_PORT->DOECLR31_0 = PING_PIN;

            // Route Ping IO pin to Echo Timer input capture (CC0 & CC1)
            IOMUX->SECCFG.PINCM[PING_PINCM] = PING_ECHO_FUNC;

            // Set Buffer direction to input
            BUF_PORT->DOUTCLR31_0 = BUF_DIR_PIN;
            // Enable Echo Timer
            ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;

            break;
        }
        case DL_TIMER_IIDX_LOAD:
        {
            // Advance Sensor Index
            if (++g_ult_idx >= ULT_COUNT) {
                g_ult_idx = 0;
            }
            break;
        }
        default:
            return;
    }
}

void ULT_ECHO_TIM_INST_IRQHandler(void)
{
    switch (getPendingInterrupt(ULT_ECHO_TIM_INST)) {
        case DL_TIMER_IIDX_CC1_UP:
            // Store CC1 capture as pulse width for current sensor
            g_ult_pw_us[g_ult_idx] = getCaptureCompareValue(
                ULT_ECHO_TIM_INST,  DL_TIMER_CC_1_INDEX);
            // Disable Echo Timer
            ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);
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

