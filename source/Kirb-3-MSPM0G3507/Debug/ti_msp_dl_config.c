/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.c =============
 *  Configured MSPM0 DriverLib module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */

#include "ti_msp_dl_config.h"

DL_TimerA_backupConfig gMOTOR_PWMBackup;
DL_TimerG_backupConfig gQEI_0Backup;
DL_TimerA_backupConfig gRC_TIM0Backup;
DL_TimerG_backupConfig gRC_TIM1Backup;
DL_TimerG_backupConfig gPING_SCHED_TIMBackup;

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform any initialization needed before using any board APIs
 */
SYSCONFIG_WEAK void SYSCFG_DL_init(void)
{
    SYSCFG_DL_initPower();
    SYSCFG_DL_GPIO_init();
    /* Module-Specific Initializations*/
    SYSCFG_DL_SYSCTL_init();
    SYSCFG_DL_MOTOR_PWM_init();
    SYSCFG_DL_QEI_0_init();
    SYSCFG_DL_RC_TIM0_init();
    SYSCFG_DL_RC_TIM1_init();
    SYSCFG_DL_ECHO_TIM_init();
    SYSCFG_DL_PING_SCHED_TIM_init();
    /* Ensure backup structures have no valid state */
	gMOTOR_PWMBackup.backupRdy 	= false;
	gQEI_0Backup.backupRdy 	= false;
	gRC_TIM0Backup.backupRdy 	= false;
	gRC_TIM1Backup.backupRdy 	= false;
	gPING_SCHED_TIMBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(MOTOR_PWM_INST, &gMOTOR_PWMBackup);
	retStatus &= DL_TimerG_saveConfiguration(QEI_0_INST, &gQEI_0Backup);
	retStatus &= DL_TimerA_saveConfiguration(RC_TIM0_INST, &gRC_TIM0Backup);
	retStatus &= DL_TimerG_saveConfiguration(RC_TIM1_INST, &gRC_TIM1Backup);
	retStatus &= DL_TimerG_saveConfiguration(PING_SCHED_TIM_INST, &gPING_SCHED_TIMBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(MOTOR_PWM_INST, &gMOTOR_PWMBackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(QEI_0_INST, &gQEI_0Backup, false);
	retStatus &= DL_TimerA_restoreConfiguration(RC_TIM0_INST, &gRC_TIM0Backup, false);
	retStatus &= DL_TimerG_restoreConfiguration(RC_TIM1_INST, &gRC_TIM1Backup, false);
	retStatus &= DL_TimerG_restoreConfiguration(PING_SCHED_TIM_INST, &gPING_SCHED_TIMBackup, false);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(MOTOR_PWM_INST);
    DL_TimerG_reset(QEI_0_INST);
    DL_TimerA_reset(RC_TIM0_INST);
    DL_TimerG_reset(RC_TIM1_INST);
    DL_TimerG_reset(ECHO_TIM_INST);
    DL_TimerG_reset(PING_SCHED_TIM_INST);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(MOTOR_PWM_INST);
    DL_TimerG_enablePower(QEI_0_INST);
    DL_TimerA_enablePower(RC_TIM0_INST);
    DL_TimerG_enablePower(RC_TIM1_INST);
    DL_TimerG_enablePower(ECHO_TIM_INST);
    DL_TimerG_enablePower(PING_SCHED_TIM_INST);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_MOTOR_PWM_C0_IOMUX,GPIO_MOTOR_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_MOTOR_PWM_C0_PORT, GPIO_MOTOR_PWM_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_MOTOR_PWM_C1_IOMUX,GPIO_MOTOR_PWM_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_MOTOR_PWM_C1_PORT, GPIO_MOTOR_PWM_C1_PIN);

    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_0_PHA_IOMUX,GPIO_QEI_0_PHA_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_0_PHB_IOMUX,GPIO_QEI_0_PHB_IOMUX_FUNC);

    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM0_C0_IOMUX,GPIO_RC_TIM0_C0_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM0_C2_IOMUX,GPIO_RC_TIM0_C2_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM0_C3_IOMUX,GPIO_RC_TIM0_C3_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM1_C0_IOMUX,GPIO_RC_TIM1_C0_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_ECHO_TIM_C0_IOMUX,GPIO_ECHO_TIM_C0_IOMUX_FUNC);

    DL_GPIO_initDigitalInputFeatures(RC_IN_CH5_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(RC_IN_CH6_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(PING_BUF_EN_IOMUX);

    DL_GPIO_initDigitalOutput(PING_BUF_DIR_IOMUX);

    DL_GPIO_initDigitalOutput(PING_MUX0_EN_IOMUX);

    DL_GPIO_initDigitalOutput(PING_MUX1_EN_IOMUX);

    DL_GPIO_initDigitalOutput(PING_MUX_SEL0_IOMUX);

    DL_GPIO_initDigitalOutput(PING_MUX_SEL1_IOMUX);

    DL_GPIO_initDigitalOutput(PING_MUX_SEL2_IOMUX);

    DL_GPIO_initDigitalOutput(PING_PING_BUS_IOMUX);

    DL_GPIO_initDigitalOutput(QEI_QEI1_A_IOMUX);

    DL_GPIO_initDigitalOutput(QEI_QEI1_B_IOMUX);

    DL_GPIO_clearPins(GPIOA, PING_MUX0_EN_PIN |
		PING_MUX1_EN_PIN |
		PING_MUX_SEL1_PIN |
		PING_MUX_SEL2_PIN |
		PING_PING_BUS_PIN);
    DL_GPIO_enableOutput(GPIOA, PING_MUX0_EN_PIN |
		PING_MUX1_EN_PIN |
		PING_MUX_SEL1_PIN |
		PING_MUX_SEL2_PIN |
		PING_PING_BUS_PIN);
    DL_GPIO_clearPins(GPIOB, PING_BUF_EN_PIN |
		PING_BUF_DIR_PIN |
		PING_MUX_SEL0_PIN |
		QEI_QEI1_A_PIN |
		QEI_QEI1_B_PIN);
    DL_GPIO_enableOutput(GPIOB, PING_BUF_EN_PIN |
		PING_BUF_DIR_PIN |
		PING_MUX_SEL0_PIN |
		QEI_QEI1_A_PIN |
		QEI_QEI1_B_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_13_EDGE_FALL);
    DL_GPIO_setUpperPinsPolarity(GPIOB, DL_GPIO_PIN_20_EDGE_FALL);
    DL_GPIO_clearInterruptStatus(GPIOB, RC_IN_CH5_PIN |
		RC_IN_CH6_PIN);
    DL_GPIO_enableInterrupt(GPIOB, RC_IN_CH5_PIN |
		RC_IN_CH6_PIN);

}


SYSCONFIG_WEAK void SYSCFG_DL_SYSCTL_init(void)
{

	//Low Power Mode is configured to be SLEEP0
    DL_SYSCTL_setBORThreshold(DL_SYSCTL_BOR_THRESHOLD_LEVEL_0);

    DL_SYSCTL_setSYSOSCFreq(DL_SYSCTL_SYSOSC_FREQ_BASE);
    DL_SYSCTL_enableMFCLK();
    DL_SYSCTL_setULPCLKDivider(DL_SYSCTL_ULPCLK_DIV_1);
    DL_SYSCTL_setMCLKDivider(DL_SYSCTL_MCLK_DIVIDER_DISABLE);

}


/*
 * Timer clock configuration to be sourced by  / 1 (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerA_ClockConfig gMOTOR_PWMClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 31U
};

static const DL_TimerA_PWMConfig gMOTOR_PWMConfig = {
    .pwmMode = DL_TIMER_PWM_MODE_EDGE_ALIGN_UP,
    .period = 20000,
    .isTimerWithFourCC = false,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_MOTOR_PWM_init(void) {

    DL_TimerA_setClockConfig(
        MOTOR_PWM_INST, (DL_TimerA_ClockConfig *) &gMOTOR_PWMClockConfig);

    DL_TimerA_initPWMMode(
        MOTOR_PWM_INST, (DL_TimerA_PWMConfig *) &gMOTOR_PWMConfig);

    // Set Counter control to the smallest CC index being used
    DL_TimerA_setCounterControl(MOTOR_PWM_INST,DL_TIMER_CZC_CCCTL0_ZCOND,DL_TIMER_CAC_CCCTL0_ACOND,DL_TIMER_CLC_CCCTL0_LCOND);

    DL_TimerA_setCaptureCompareOutCtl(MOTOR_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_HIGH,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_0_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(MOTOR_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, DL_TIMERA_CAPTURE_COMPARE_0_INDEX);
    DL_TimerA_setCaptureCompareValue(MOTOR_PWM_INST, 1500, DL_TIMER_CC_0_INDEX);

    DL_TimerA_setCaptureCompareOutCtl(MOTOR_PWM_INST, DL_TIMER_CC_OCTL_INIT_VAL_HIGH,
		DL_TIMER_CC_OCTL_INV_OUT_DISABLED, DL_TIMER_CC_OCTL_SRC_FUNCVAL,
		DL_TIMERA_CAPTURE_COMPARE_1_INDEX);

    DL_TimerA_setCaptCompUpdateMethod(MOTOR_PWM_INST, DL_TIMER_CC_UPDATE_METHOD_ZERO_EVT, DL_TIMERA_CAPTURE_COMPARE_1_INDEX);
    DL_TimerA_setCaptureCompareValue(MOTOR_PWM_INST, 1500, DL_TIMER_CC_1_INDEX);

    DL_TimerA_enableClock(MOTOR_PWM_INST);


    
    DL_TimerA_setCCPDirection(MOTOR_PWM_INST , DL_TIMER_CC0_OUTPUT | DL_TIMER_CC1_OUTPUT );
    DL_TimerA_enableShadowFeatures(MOTOR_PWM_INST);


}


static const DL_TimerG_ClockConfig gQEI_0ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};


SYSCONFIG_WEAK void SYSCFG_DL_QEI_0_init(void) {

    DL_TimerG_setClockConfig(
        QEI_0_INST, (DL_TimerG_ClockConfig *) &gQEI_0ClockConfig);

    DL_TimerG_configQEI(QEI_0_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_0_INDEX);
    DL_TimerG_configQEI(QEI_0_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setLoadValue(QEI_0_INST, 65535);
    DL_TimerG_enableClock(QEI_0_INST);
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

SYSCONFIG_WEAK void SYSCFG_DL_RC_TIM0_init(void) {

    DL_TimerA_setClockConfig(RC_TIM0_INST,
        (DL_TimerA_ClockConfig *) &gRC_TIM0ClockConfig);

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

/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerG_ClockConfig gRC_TIM1ClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 31U
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * RC_TIM1_INST_LOAD_VALUE = (65ms * 1000000 Hz) - 1
 */

SYSCONFIG_WEAK void SYSCFG_DL_RC_TIM1_init(void) {

    DL_TimerG_setClockConfig(RC_TIM1_INST,
        (DL_TimerG_ClockConfig *) &gRC_TIM1ClockConfig);

    DL_TimerG_setLoadValue(RC_TIM1_INST,64999);

    DL_TimerG_setCounterMode(RC_TIM1_INST,DL_TIMER_COUNT_MODE_UP);

    DL_TimerG_setCounterRepeatMode(RC_TIM1_INST,DL_TIMER_REPEAT_MODE_ENABLED);

    DL_TimerG_setCounterValueAfterEnable(RC_TIM1_INST,DL_TIMER_COUNT_AFTER_EN_ZERO);

    DL_TimerG_setCaptureCompareCtl(RC_TIM1_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_TRIG_RISE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_RISE),
    DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareInput(RC_TIM1_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareCtl(RC_TIM1_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_NONE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_FALL),
    DL_TIMER_CC_1_INDEX);

    DL_TimerG_setCaptureCompareInput(RC_TIM1_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX_PAIR, DL_TIMER_CC_1_INDEX);


    DL_TimerG_setCounterControl(RC_TIM1_INST,
        DL_TIMER_CZC_CCCTL0_ZCOND,
        DL_TIMER_CAC_CCCTL0_ACOND,
        DL_TIMER_CLC_CCCTL0_LCOND
    );

    DL_TimerG_startCounter(RC_TIM1_INST);

    DL_TimerG_enableInterrupt(RC_TIM1_INST , DL_TIMERG_INTERRUPT_CC0_UP_EVENT |
		DL_TIMERG_INTERRUPT_CC1_UP_EVENT);

    DL_TimerG_enableClock(RC_TIM1_INST);

}

/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   32000000 Hz = 32000000 Hz / (1 * (0 + 1))
 */
static const DL_TimerG_ClockConfig gECHO_TIMClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * ECHO_TIM_INST_LOAD_VALUE = (0 ms * 32000000 Hz) - 1
 */

SYSCONFIG_WEAK void SYSCFG_DL_ECHO_TIM_init(void) {

    DL_TimerG_setClockConfig(ECHO_TIM_INST,
        (DL_TimerG_ClockConfig *) &gECHO_TIMClockConfig);

    DL_TimerG_setLoadValue(ECHO_TIM_INST,-1);

    DL_TimerG_setCounterMode(ECHO_TIM_INST,DL_TIMER_COUNT_MODE_UP);

    DL_TimerG_setCounterRepeatMode(ECHO_TIM_INST,DL_TIMER_REPEAT_MODE_ENABLED);

    DL_TimerG_setCounterValueAfterEnable(ECHO_TIM_INST,DL_TIMER_COUNT_AFTER_EN_ZERO);

    DL_TimerG_setCaptureCompareCtl(ECHO_TIM_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_TRIG_RISE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_RISE),
    DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareInput(ECHO_TIM_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX, DL_TIMER_CC_0_INDEX);

    DL_TimerG_setCaptureCompareCtl(ECHO_TIM_INST,
    DL_TIMER_CC_MODE_CAPTURE, (DL_TIMER_CC_ZCOND_NONE | DL_TIMER_CC_ACOND_TIMCLK | DL_TIMER_CC_CCOND_TRIG_FALL),
    DL_TIMER_CC_1_INDEX);

    DL_TimerG_setCaptureCompareInput(ECHO_TIM_INST,
        DL_TIMER_CC_INPUT_INV_NOINVERT,DL_TIMER_CC_IN_SEL_CCPX_PAIR, DL_TIMER_CC_1_INDEX);


    DL_TimerG_setCounterControl(ECHO_TIM_INST,
        DL_TIMER_CZC_CCCTL0_ZCOND,
        DL_TIMER_CAC_CCCTL0_ACOND,
        DL_TIMER_CLC_CCCTL0_LCOND
    );

    DL_TimerG_enableInterrupt(ECHO_TIM_INST , DL_TIMERG_INTERRUPT_CC1_UP_EVENT);

    DL_TimerG_enableClock(ECHO_TIM_INST);

}


/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerG_ClockConfig gPING_SCHED_TIMClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 31U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * PING_SCHED_TIM_INST_LOAD_VALUE = (20 ms * 1000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gPING_SCHED_TIMTimerConfig = {
    .period     = PING_SCHED_TIM_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC_UP,
    .startTimer = DL_TIMER_START,
};

SYSCONFIG_WEAK void SYSCFG_DL_PING_SCHED_TIM_init(void) {

    DL_TimerG_setClockConfig(PING_SCHED_TIM_INST,
        (DL_TimerG_ClockConfig *) &gPING_SCHED_TIMClockConfig);

    DL_TimerG_initTimerMode(PING_SCHED_TIM_INST,
        (DL_TimerG_TimerConfig *) &gPING_SCHED_TIMTimerConfig);
    DL_TimerG_enableInterrupt(PING_SCHED_TIM_INST , DL_TIMERG_INTERRUPT_CC0_UP_EVENT |
		DL_TIMERG_INTERRUPT_LOAD_EVENT |
		DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(PING_SCHED_TIM_INST);





}


