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
  //Constants still need to be defined

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
  SYSCTL->SOCLOCK.BORTHRESHOLD = (uint32_t) SYSCTL_BORTHRESHOLD_LEVEL_BORMIN;

  
}



void PWM_init(void)
{
  gptimer->CLKSEL = (uint32_t)(config->clockSel);

  gptimer->CLKDIV = (uint32_t)(config->divideRatio);

  gptimer->COMMONREGS.CPS = (config->prescale);
}  





void RC_timer0_init(void)
{

}



void RC_timer1_init(void)
{

}




<<<<<<< HEAD


RC_timer1_init()
{
    
}

// git pull
// git add .
// git commit -m 'message here'
// git push
=======
>>>>>>> 04f1af6da53a3eaf02e1da99cedda0079a26c278
