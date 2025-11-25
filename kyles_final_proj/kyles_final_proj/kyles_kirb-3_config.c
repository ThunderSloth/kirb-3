//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------

#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "clock.h"
#include "LaunchPad.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void config_init(void)
void power_init(void);
void GPIO_init(void);
void PWM_init(void);
void RC_timer0_init(void);
void RC_timer1_init(void);


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

// testing
/*

SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    // Module-Specific Initializations
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_MOTOR_PWM_init();
    SYSCFG_DL_RC_TIM0_init();
    SYSCFG_DL_RC_TIM1_init();
    //Ensure backup structures have no valid state 
	gMOTOR_PWMBackup.backupRdy 	= false;
	gRC_TIM0Backup.backupRdy 	= false;
	gRC_TIM1Backup.backupRdy 	= false;

}


*/

void config_init(void)
{
    power_init();
    GPIO_init();
    PWM_init();
    RC_timer0_init();
    RC_timer1_init();


}



void power_init(void)
{
    // Reset two GPIO peripherals
    // DL_GPIO_reset(GPIOA);
    GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                            GPIO_RSTCTL_RESETSTKYCLR_CLR |
                            GPIO_RSTCTL_RESETASSERT_ASSERT);
    //DL_GPIO_reset(GPIOB);
    GPIOB->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                            GPIO_RSTCTL_RESETSTKYCLR_CLR |
                            GPIO_RSTCTL_RESETASSERT_ASSERT);

    //   DL_TimerA_reset(MOTOR_PWM_INST);
    MOTOR_PWM_INST->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
                                    GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
                                    GPTIMER_RSTCTL_RESETASSERT_ASSERT);

    //    DL_TimerA_reset(RC_TIM0_INST);
    RC_TIM0_INST->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
                                    GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
                                    GPTIMER_RSTCTL_RESETASSERT_ASSERT);

    //   DL_TimerG_reset(RC_TIM1_INST);
    RC_TIM1_INST->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
                                    GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
                                    GPTIMER_RSTCTL_RESETASSERT_ASSERT);

    // Enable power to two GPIO peripherals
    GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
    GPIOB->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);

    //DL_TimerA_enablePower(MOTOR_PWM_INST);
    MOTOR_PWM_INST->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);

    // DL_TimerA_enablePower(RC_TIM0_INST);
    RC_TIM0_INST->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);

    // DL_TimerG_enablePower(RC_TIM1_INST);
    RC_TIM1_INST->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | GPTIMER_PWREN_ENABLE_ENABLE);

    clock_delay(24);
}


void GPIO_init(void)
{

  //Configure PWM motor output (TIMA1_C0) on pin PB2 
  IOMUX->SECCFG.PINCM[GPIO_MOTOR_PWM_C0_IOMUX] = GPIO_MOTOR_PWM_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED;
  //Enable output on pin PB2
  GPIO_MOTOR_PWM_C0_PORT->DOESET31_0 = GPIO_MOTOR_PWM_C0_PIN;
  
  //Configure PWM motor output (TIMA1_C1) on pin PB3 
  IOMUX->SECCFG.PINCM[GPIO_MOTOR_PWM_C1_IOMUX] = GPIO_MOTOR_PWM_C1_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED;  
  //Enable output on pin PB3
  GPIO_MOTOR_PWM_C1_PORT->DOESET31_0 = GPIO_MOTOR_PWM_C1_PIN;

  //Configure RC input (TIMA0_C0N) on pin PA8 
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM0_C0_IOMUX] =
      GPIO_RC_TIM0_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

  //Configure RC input (TIM0_C2N) on pin PA15 
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM0_C2_IOMUX] =
      GPIO_RC_TIM0_C2_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

  //Configure RC input (TIMA0_C3) on pin PA25
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM0_C3_IOMUX] =
      GPIO_RC_TIM0_C3_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;
  
  //Configure RC input (TIMA1_C0) on pin PA28
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM1_C0_IOMUX] =
      GPIO_RC_TIM1_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

}


/*
1. In the TIMx.CTRCTL register, set the desired counter control settings for:
    a. Up-counting (CM = 2) or down-counting mode (CM = 0) and counter value after enable (CVAE) (see as
        described in Section 27.2.2)
    b. Zero (CZC), advance (CAC), and load control (CLC) to specify what condition controls zeroing,
        advancing, or loading the counter
    c. Repeat or one-shot mode (REPEAT)

2. Set the TIMx.LOAD value to configure the PWM period.

3. Set the TIMx.CC_xy[0/1] value to configure the duty cycle.

4. Set TIMx.CCCTL_xy[0/1].COC = 1 for compare mode.

5. Configure CCP as an output for the CC block by setting respective bit in the CCPD registers. For instance, if
TIMx Channel 0 is an output, set CCPD.C0CCP0 = 1.

6. In TIMx.CCACT_xy[0/1], set the CCP output action settings for compare events, zero events, load events,
software force action, or fault events (TIMA only).

7. In TIMx.OCTL_xy[0/1], set CCPO = 0 to select the signal generator output.

8. Enable the corresponding CCP output by setting ODIS.C0CCPn to 1 for the corresponding counter n.

9. Configure polarity of the signal using the CCPOINV bit, and configure CCPIV to specify the CCP output state
while disabled.

10. Enable the counter by setting TIMx.CTRCTL.EN = 1.
*/
void PWM_init(void)
{
  IOMUX->SECCFG.PINCM[GPIO_MOTOR_PWM_C0_IOMUX] = GPIO_MOTOR_PWM_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED;

  GPIO_MOTOR_PWM_C0_PORT->DOESET31_0 = GPIO_MOTOR_PWM_C0_PIN;
}  



/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerA_ClockConfig gRC_TIM0ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 31U
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * RC_TIM0_INST_LOAD_VALUE = (65ms * 1000000 Hz) - 1
 */

void RC_timer0_init(void)
{
    DL_TimerA_setClockConfig(RC_TIM0_INST,
        (DL_TimerA_ClockConfig *) &gRC_TIM0ClockConfig);

    void DL_Timer_setClockConfig(
    GPTIMER_Regs *gptimer, const DL_Timer_ClockConfig *config)

    gptimer->CLKSEL = (uint32_t)(config->clockSel);

    gptimer->CLKDIV = (uint32_t)(config->divideRatio);

    gptimer->COMMONREGS.CPS = (config->prescale);


    DL_TimerA_setLoadValue(RC_TIM0_INST,64999);

    DL_TimerA_setCounterMode(RC_TIM0_INST,DL_TIMER_COUNT_MODE_UP);

    DL_TimerA_setCounterRepeatMode(RC_TIM0_INST,DL_TIMER_REPEAT_MODE_ENABLED);

    DL_TimerA_setCounterValueAfterEnable(RC_TIM0_INST,DL_TIMER_COUNT_AFTER_EN_ZERO);

    DL_TimerA_setCaptureCompareCtl(RC_TIM0_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_TRIG_RISE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_RISE),
    DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareInput(RC_TIM0_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareCtl(RC_TIM0_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_NONE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_FALL),
    DL_TIMER_CC_1_INDEX);

    DL_TimerA_setCaptureCompareInput(RC_TIM0_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX_PAIR, DL_TIMER_CC_1_INDEX);

    DL_TimerA_setCaptureCompareCtl(RC_TIM0_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_NONE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_FALL),
    DL_TIMER_CC_2_INDEX);

    DL_TimerA_setCaptureCompareInput(RC_TIM0_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX, DL_TIMER_CC_2_INDEX);

    DL_TimerA_setCaptureCompareCtl(RC_TIM0_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_NONE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_FALL),
    DL_TIMER_CC_3_INDEX);

    DL_TimerA_setCaptureCompareInput(RC_TIM0_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX, DL_TIMER_CC_3_INDEX);


    DL_TimerA_setCounterControl(RC_TIM0_INST,
        DL_TIMER_CZC_CCCTL0_ZCOND,
        DL_TIMER_CAC_CCCTL0_ACOND,
        DL_TIMER_CLC_CCCTL0_LCOND
    );

    DL_TimerA_startCounter(RC_TIM0_INST);

    DL_TimerA_enableInterrupt(RC_TIM0_INST , DL_TIMERA_INTERRUPT_CC1_UP_EVENT |
		DL_TIMERA_INTERRUPT_CC2_UP_EVENT |
		DL_TIMERA_INTERRUPT_CC3_UP_EVENT);

    DL_TimerA_enableClock(RC_TIM0_INST);
}



void RC_timer1_init(void)
{

}






// git pull
// git add .
// git commit -m 'message here'
// git push
