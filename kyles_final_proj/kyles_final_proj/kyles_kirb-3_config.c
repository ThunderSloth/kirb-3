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
void clock_init(void);
void PWM_init(void);
void RC_timer0_init(void);
void RC_timer1_init(void);


//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


// GPIO_init() constants
//------------------------------------------------------------------------------
#define GPIO_MOTOR_PWM_C0_IOMUX                                  (IOMUX_PINCM15)
#define GPIO_MOTOR_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM15_PF_TIMA1_CCP0
#define GPIO_MOTOR_PWM_C0_PIN                                       (0x00000004)

#define GPIO_MOTOR_PWM_C1_IOMUX                                  (IOMUX_PINCM16)
#define GPIO_MOTOR_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM16_PF_TIMA1_CCP1
#define GPIO_MOTOR_PWM_C1_PIN                                       (0x00000008)

#define GPIO_RC_TIM0_C0_IOMUX                                    (IOMUX_PINCM19)
#define GPIO_RC_TIM0_C0_IOMUX_FUNC                   IOMUX_PINCM19_PF_TIMA0_CCP0

#define GPIO_RC_TIM0_C2_IOMUX                                    (IOMUX_PINCM37)
#define GPIO_RC_TIM0_C2_IOMUX_FUNC                   IOMUX_PINCM37_PF_TIMA0_CCP2

#define GPIO_RC_TIM0_C3_IOMUX                                    (IOMUX_PINCM55)
#define GPIO_RC_TIM0_C3_IOMUX_FUNC                   IOMUX_PINCM55_PF_TIMA0_CCP3

#define GPIO_RC_TIM1_C0_IOMUX                                     (IOMUX_PINCM3)
#define GPIO_RC_TIM1_C0_IOMUX_FUNC                    IOMUX_PINCM3_PF_TIMG7_CCP0

#define RC_IN_CH5_IOMUX                                          (IOMUX_PINCM30)
#define RC_IN_CH6_IOMUX                                          (IOMUX_PINCM48)

#define RC_IN_PORT                                                       (GPIOB)

#define RC_IN_CH5_PIN                                               (0x00002000)
#define RC_IN_CH6_PIN                                               (0x00100000)
//------------------------------------------------------------------------------
#define GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE                 ((uint32_t)0x00000008U) 

#define DL_SYSCTL_MCLK_DIVIDER_DISABLE                                     (0x0)




//------------------------------------------------------------------------------
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
    clock_init();
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

  //Configure RC input (TIMA0_C0) on pin PA8 
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM0_C0_IOMUX] =
      GPIO_RC_TIM0_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

  //Configure RC input (TIM0_C2) on pin PA15 
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM0_C2_IOMUX] =
      GPIO_RC_TIM0_C2_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

  //Configure RC input (TIMA0_C3) on pin PA25
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM0_C3_IOMUX] =
      GPIO_RC_TIM0_C3_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;
  
  //Configure RC input (TIMA1_C0) on pin PA28
  IOMUX->SECCFG.PINCM[GPIO_RC_TIM1_C0_IOMUX] =
      GPIO_RC_TIM1_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_INENA_ENABLE;

  //Configure pin PB13 as a GPIO input
  /* GPIO functionality is always a pin function of 0x00000001 */
  IOMUX->SECCFG.PINCM[RC_IN_CH5_IOMUX] =
      IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |
      ((uint32_t) 0x00000001) | (uint32_t) IOMUX_PINCM_INV_DISABLE |
      (uint32_t) (IOMUX_PINCM_PIPU_DISABLE | IOMUX_PINCM_PIPD_DISABLE) | (uint32_t) IOMUX_PINCM_HYSTEN_DISABLE |
      ((uint32_t) IOMUX_PINCM_WUEN_DISABLE & IOMUX_PINCM_WCOMP_MASK);
  IOMUX->SECCFG.PINCM[RC_IN_CH5_IOMUX] |=
      ((uint32_t) IOMUX_PINCM_WUEN_DISABLE & IOMUX_PINCM_WUEN_MASK);

  //Configure pin PB20 as a GPIO input
  /* GPIO functionality is always a pin function of 0x00000001 */
  IOMUX->SECCFG.PINCM[RC_IN_CH6_IOMUX] =
      IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |
      ((uint32_t) 0x00000001) | (uint32_t) IOMUX_PINCM_INV_DISABLE |
      (uint32_t) (IOMUX_PINCM_PIPU_DISABLE | IOMUX_PINCM_PIPD_DISABLE) | (uint32_t) IOMUX_PINCM_HYSTEN_DISABLE |
      ((uint32_t) IOMUX_PINCM_WUEN_DISABLE & IOMUX_PINCM_WCOMP_MASK);
  IOMUX->SECCFG.PINCM[RC_IN_CH6_IOMUX] |=
      ((uint32_t) IOMUX_PINCM_WUEN_DISABLE & IOMUX_PINCM_WUEN_MASK);
    
  //Sets ploarity for lower bits
  RC_IN_PORT->POLARITY15_0 |= GPIO_POLARITY15_0_DIO13_FALL;
  //Sets polarity for upper bits
  RC_IN_PORT->POLARITY31_16 |= GPIO_POLARITY31_16_DIO20_FALL;
  //Clears interrupts
  RC_IN_PORT->CPU_INT.ICLR |= (RC_IN_CH5_PIN | RC_IN_CH6_PIN);
  //Enables interrupts
  RC_IN_PORT->CPU_INT.IMASK |= (RC_IN_CH5_PIN |RC_IN_CH6_PIN);


}



void clock_init(void)
{

  //Sets BOR threshold at minimum level (does not activate)
  SYSCTL->SOCLOCK.BORTHRESHOLD = (uint32_t) SYSCTL_BORTHRESHOLD_LEVEL_BORMIN;
  //Sets the system oscillator to 32MHz
  update_Reg(&SYSCTL->SOCLOCK.SYSOSCCFG, (uint32_t) SYSCTL_SYSOSCCFG_FREQ_SYSOSCBASE,
        SYSCTL_SYSOSCCFG_FREQ_MASK);
  //Enables the medium frequency clock (4Mhz)
  SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_USEMFTICK_ENABLE;
  //Sets up ultra low power clock (not divided)
  update_Reg(&SYSCTL->SOCLOCK.MCLKCFG, (uint32_t) SYSCTL_MCLKCFG_UDIV_NODIVIDE,
        SYSCTL_MCLKCFG_UDIV_MASK);
  //Disable main clock divider 
  updateReg(&SYSCTL->SOCLOCK.MCLKCFG, (uint32_t) DL_SYSCTL_MCLK_DIVIDER_DISABLE,
        SYSCTL_MCLKCFG_MDIV_MASK);

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
  
  
}  

<<<<<<< HEAD

=======
<<<<<<< HEAD

=======
>>>>>>> 2e291750a239ac3764df4a580da9d48cbea864d2
>>>>>>> 5e52a59f2db1fcb709821a65fa52079988ca4dfa


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


<<<<<<< HEAD

void RC_timer1_init(void)
{

}






RC_timer1_init()
=======
void RC_timer1_init()
>>>>>>> 2e291750a239ac3764df4a580da9d48cbea864d2
{
    
}





typedef struct {
    /* Selects timer module clock source DL_TIMER_CLOCK*/
    DL_TIMER_CLOCK clockSel;
    /* Selects the timer module clock divide ratio DL_TIMER_CLOCK_DIVIDE */
    DL_TIMER_CLOCK_DIVIDE divideRatio;
    /* Selects the timer module clock prescaler. Valid range 0-255 */
    uint8_t prescale;
} DL_Timer_ClockConfig;

typedef enum {
    /*! Selects BUSCLK as clock source */
    DL_TIMER_CLOCK_BUSCLK = GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE,
    /*! Selects 2X BUSCLK as clock source */
    DL_TIMER_CLOCK_2X_BUSCLK = GPTIMER_CLKSEL_BUS2XCLK_SEL_ENABLE,
    /*! Selects MFCLK as clock source */
    DL_TIMER_CLOCK_MFCLK = GPTIMER_CLKSEL_MFCLK_SEL_ENABLE,
    /*! Selects LFCLK as clock source */
    DL_TIMER_CLOCK_LFCLK = GPTIMER_CLKSEL_LFCLK_SEL_ENABLE,
    /*! Disables selected clock source */
    DL_TIMER_CLOCK_DISABLE = GPTIMER_CLKSEL_LFCLK_SEL_DISABLE,
} DL_TIMER_CLOCK;



// git pull
// git add .
// git commit -m 'message here'
// git push
<<<<<<< HEAD
=======
>>>>>>> 04f1af6da53a3eaf02e1da99cedda0079a26c278

 void update_Reg(volatile uint32_t *reg, uint32_t value, uint32_t mask)
{
    uint32_t temp_reg;

    temp_reg  = *reg;
    temp_reg  = temp_reg & ~mask;
    *reg = temp_reg | (value & mask);
}
=======
>>>>>>> 5e52a59f2db1fcb709821a65fa52079988ca4dfa
