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

}



void clock_init(void)
{

}



void PWM_init(void)
{
  IOMUX->SECCFG.PINCM[GPIO_MOTOR_PWM_C0_IOMUX] = GPIO_MOTOR_PWM_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED;

  GPIO_MOTOR_PWM_C0_PORT->DOESET31_0 = GPIO_MOTOR_PWM_C0_PIN;
}  

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
