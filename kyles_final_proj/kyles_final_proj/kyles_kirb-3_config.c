//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <ti/devices/msp/msp.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------

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



power_init()
{

}



GPIO_init()
{
  IOMUX->SECCFG.PINCM[GPIO_MOTOR_PWM_C0_IOMUX] = GPIO_MOTOR_PWM_C0_IOMUX_FUNC | IOMUX_PINCM_PC_CONNECTED;

  GPIO_MOTOR_PWM_C0_PORT->DOESET31_0 = GPIO_MOTOR_PWM_C0_PIN;
}  

}



clock_init();
{

}



PWM_init()
{

}



RC_timer0_init()
{
    // /ti/mspm0_sdk_2_08_00_03/source/ti/driverlib/dl_timer.c
}



RC_timer1_init()
{
    
}

// git pull
// git add .
// git commit -m 'message here'
// git push