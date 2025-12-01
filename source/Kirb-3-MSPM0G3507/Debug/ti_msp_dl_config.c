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
DL_TimerG_backupConfig gQEI_MOTOR1Backup;
DL_TimerA_backupConfig gRC_TIM0Backup;
DL_TimerG_backupConfig gRC_TIM1Backup;
DL_SPI_backupConfig gSPI_FRAMBackup;

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
    SYSCFG_DL_QEI_MOTOR1_init();
    SYSCFG_DL_RC_TIM0_init();
    SYSCFG_DL_RC_TIM1_init();
    SYSCFG_DL_ECHO_TIM_init();
    SYSCFG_DL_I2C_IMU_init();
    SYSCFG_DL_UART_ESP32_init();
    SYSCFG_DL_SPI_FRAM_init();
    SYSCFG_DL_V_MEAS_init();
    SYSCFG_DL_VREF_init();
    /* Ensure backup structures have no valid state */
	gMOTOR_PWMBackup.backupRdy 	= false;
	gQEI_MOTOR1Backup.backupRdy 	= false;
	gRC_TIM0Backup.backupRdy 	= false;
	gRC_TIM1Backup.backupRdy 	= false;

	gSPI_FRAMBackup.backupRdy 	= false;

}
/*
 * User should take care to save and restore register configuration in application.
 * See Retention Configuration section for more details.
 */
SYSCONFIG_WEAK bool SYSCFG_DL_saveConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_saveConfiguration(MOTOR_PWM_INST, &gMOTOR_PWMBackup);
	retStatus &= DL_TimerG_saveConfiguration(QEI_MOTOR1_INST, &gQEI_MOTOR1Backup);
	retStatus &= DL_TimerA_saveConfiguration(RC_TIM0_INST, &gRC_TIM0Backup);
	retStatus &= DL_TimerG_saveConfiguration(RC_TIM1_INST, &gRC_TIM1Backup);
	retStatus &= DL_SPI_saveConfiguration(SPI_FRAM_INST, &gSPI_FRAMBackup);

    return retStatus;
}


SYSCONFIG_WEAK bool SYSCFG_DL_restoreConfiguration(void)
{
    bool retStatus = true;

	retStatus &= DL_TimerA_restoreConfiguration(MOTOR_PWM_INST, &gMOTOR_PWMBackup, false);
	retStatus &= DL_TimerG_restoreConfiguration(QEI_MOTOR1_INST, &gQEI_MOTOR1Backup, false);
	retStatus &= DL_TimerA_restoreConfiguration(RC_TIM0_INST, &gRC_TIM0Backup, false);
	retStatus &= DL_TimerG_restoreConfiguration(RC_TIM1_INST, &gRC_TIM1Backup, false);
	retStatus &= DL_SPI_restoreConfiguration(SPI_FRAM_INST, &gSPI_FRAMBackup);

    return retStatus;
}

SYSCONFIG_WEAK void SYSCFG_DL_initPower(void)
{
    DL_GPIO_reset(GPIOA);
    DL_GPIO_reset(GPIOB);
    DL_TimerA_reset(MOTOR_PWM_INST);
    DL_TimerG_reset(QEI_MOTOR1_INST);
    DL_TimerA_reset(RC_TIM0_INST);
    DL_TimerG_reset(RC_TIM1_INST);
    DL_TimerG_reset(ECHO_TIM_INST);
    DL_I2C_reset(I2C_IMU_INST);
    DL_UART_Main_reset(UART_ESP32_INST);
    DL_SPI_reset(SPI_FRAM_INST);
    DL_ADC12_reset(V_MEAS_INST);
    DL_VREF_reset(VREF);

    DL_GPIO_enablePower(GPIOA);
    DL_GPIO_enablePower(GPIOB);
    DL_TimerA_enablePower(MOTOR_PWM_INST);
    DL_TimerG_enablePower(QEI_MOTOR1_INST);
    DL_TimerA_enablePower(RC_TIM0_INST);
    DL_TimerG_enablePower(RC_TIM1_INST);
    DL_TimerG_enablePower(ECHO_TIM_INST);
    DL_I2C_enablePower(I2C_IMU_INST);
    DL_UART_Main_enablePower(UART_ESP32_INST);
    DL_SPI_enablePower(SPI_FRAM_INST);
    DL_ADC12_enablePower(V_MEAS_INST);
    DL_VREF_enablePower(VREF);
    delay_cycles(POWER_STARTUP_DELAY);
}

SYSCONFIG_WEAK void SYSCFG_DL_GPIO_init(void)
{

    DL_GPIO_initPeripheralOutputFunction(GPIO_MOTOR_PWM_C0_IOMUX,GPIO_MOTOR_PWM_C0_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_MOTOR_PWM_C0_PORT, GPIO_MOTOR_PWM_C0_PIN);
    DL_GPIO_initPeripheralOutputFunction(GPIO_MOTOR_PWM_C1_IOMUX,GPIO_MOTOR_PWM_C1_IOMUX_FUNC);
    DL_GPIO_enableOutput(GPIO_MOTOR_PWM_C1_PORT, GPIO_MOTOR_PWM_C1_PIN);

    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_MOTOR1_PHA_IOMUX,GPIO_QEI_MOTOR1_PHA_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_QEI_MOTOR1_PHB_IOMUX,GPIO_QEI_MOTOR1_PHB_IOMUX_FUNC);

    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM0_C0_IOMUX,GPIO_RC_TIM0_C0_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM0_C2_IOMUX,GPIO_RC_TIM0_C2_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM0_C3_IOMUX,GPIO_RC_TIM0_C3_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_RC_TIM1_C0_IOMUX,GPIO_RC_TIM1_C0_IOMUX_FUNC);
    DL_GPIO_initPeripheralInputFunction(GPIO_ECHO_TIM_C0_IOMUX,GPIO_ECHO_TIM_C0_IOMUX_FUNC);

    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IMU_IOMUX_SDA,
        GPIO_I2C_IMU_IOMUX_SDA_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_initPeripheralInputFunctionFeatures(GPIO_I2C_IMU_IOMUX_SCL,
        GPIO_I2C_IMU_IOMUX_SCL_FUNC, DL_GPIO_INVERSION_DISABLE,
        DL_GPIO_RESISTOR_NONE, DL_GPIO_HYSTERESIS_DISABLE,
        DL_GPIO_WAKEUP_DISABLE);
    DL_GPIO_enableHiZ(GPIO_I2C_IMU_IOMUX_SDA);
    DL_GPIO_enableHiZ(GPIO_I2C_IMU_IOMUX_SCL);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_UART_ESP32_IOMUX_TX, GPIO_UART_ESP32_IOMUX_TX_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_UART_ESP32_IOMUX_RX, GPIO_UART_ESP32_IOMUX_RX_FUNC);

    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_FRAM_IOMUX_SCLK, GPIO_SPI_FRAM_IOMUX_SCLK_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_FRAM_IOMUX_PICO, GPIO_SPI_FRAM_IOMUX_PICO_FUNC);
    DL_GPIO_initPeripheralInputFunction(
        GPIO_SPI_FRAM_IOMUX_POCI, GPIO_SPI_FRAM_IOMUX_POCI_FUNC);
    DL_GPIO_initPeripheralOutputFunction(
        GPIO_SPI_FRAM_IOMUX_CS0, GPIO_SPI_FRAM_IOMUX_CS0_FUNC);

    DL_GPIO_initDigitalInputFeatures(PING_TRIG_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(RC_IN_CH5_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalInputFeatures(RC_IN_CH6_IOMUX,
		 DL_GPIO_INVERSION_DISABLE, DL_GPIO_RESISTOR_NONE,
		 DL_GPIO_HYSTERESIS_DISABLE, DL_GPIO_WAKEUP_DISABLE);

    DL_GPIO_initDigitalOutput(QEI_MOTOR2_A_IOMUX);

    DL_GPIO_initDigitalOutput(QEI_MOTOR2_B_IOMUX);

    DL_GPIO_initDigitalOutput(MUX_EN0_IOMUX);

    DL_GPIO_initDigitalOutput(MUX_EN1_IOMUX);

    DL_GPIO_initDigitalOutput(MUX_SEL0_IOMUX);

    DL_GPIO_initDigitalOutput(MUX_SEL1_IOMUX);

    DL_GPIO_initDigitalOutput(MUX_SEL2_IOMUX);

    DL_GPIO_initDigitalOutput(BUF_EN_IOMUX);

    DL_GPIO_initDigitalOutput(BUF_DIR_IOMUX);

    DL_GPIO_clearPins(GPIOA, MUX_EN1_PIN |
		MUX_SEL1_PIN |
		MUX_SEL2_PIN |
		BUF_EN_PIN |
		BUF_DIR_PIN);
    DL_GPIO_enableOutput(GPIOA, MUX_EN1_PIN |
		MUX_SEL1_PIN |
		MUX_SEL2_PIN |
		BUF_EN_PIN |
		BUF_DIR_PIN);
    DL_GPIO_clearPins(GPIOB, QEI_MOTOR2_A_PIN |
		QEI_MOTOR2_B_PIN |
		MUX_EN0_PIN |
		MUX_SEL0_PIN);
    DL_GPIO_enableOutput(GPIOB, QEI_MOTOR2_A_PIN |
		QEI_MOTOR2_B_PIN |
		MUX_EN0_PIN |
		MUX_SEL0_PIN);
    DL_GPIO_setLowerPinsPolarity(GPIOB, DL_GPIO_PIN_12_EDGE_FALL);
    DL_GPIO_setUpperPinsPolarity(GPIOB, DL_GPIO_PIN_17_EDGE_FALL);
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


static const DL_TimerG_ClockConfig gQEI_MOTOR1ClockConfig = {
    .clockSel = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale = 0U
};


SYSCONFIG_WEAK void SYSCFG_DL_QEI_MOTOR1_init(void) {

    DL_TimerG_setClockConfig(
        QEI_MOTOR1_INST, (DL_TimerG_ClockConfig *) &gQEI_MOTOR1ClockConfig);

    DL_TimerG_configQEI(QEI_MOTOR1_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_0_INDEX);
    DL_TimerG_configQEI(QEI_MOTOR1_INST, DL_TIMER_QEI_MODE_2_INPUT,
        DL_TIMER_CC_INPUT_INV_NOINVERT, DL_TIMER_CC_1_INDEX);
    DL_TimerG_setLoadValue(QEI_MOTOR1_INST, 65535);
    DL_TimerG_enableClock(QEI_MOTOR1_INST);
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
static const DL_TimerG_CaptureConfig gECHO_TIMCaptureConfig = {
    .captureMode    = DL_TIMER_CAPTURE_MODE_EDGE_TIME,
    .period         = ECHO_TIM_INST_LOAD_VALUE,
    .startTimer     = DL_TIMER_STOP,
    .edgeCaptMode   = DL_TIMER_CAPTURE_EDGE_DETECTION_MODE_RISING,
    .inputChan      = DL_TIMER_INPUT_CHAN_0,
    .inputInvMode   = DL_TIMER_CC_INPUT_INV_NOINVERT,
};

SYSCONFIG_WEAK void SYSCFG_DL_ECHO_TIM_init(void) {

    DL_TimerG_setClockConfig(ECHO_TIM_INST,
        (DL_TimerG_ClockConfig *) &gECHO_TIMClockConfig);

    DL_TimerG_initCaptureMode(ECHO_TIM_INST,
        (DL_TimerG_CaptureConfig *) &gECHO_TIMCaptureConfig);
    DL_TimerG_enableClock(ECHO_TIM_INST);

}

static const DL_I2C_ClockConfig gI2C_IMUClockConfig = {
    .clockSel = DL_I2C_CLOCK_BUSCLK,
    .divideRatio = DL_I2C_CLOCK_DIVIDE_1,
};

SYSCONFIG_WEAK void SYSCFG_DL_I2C_IMU_init(void) {

    DL_I2C_setClockConfig(I2C_IMU_INST,
        (DL_I2C_ClockConfig *) &gI2C_IMUClockConfig);
    DL_I2C_setAnalogGlitchFilterPulseWidth(I2C_IMU_INST,
        DL_I2C_ANALOG_GLITCH_FILTER_WIDTH_50NS);
    DL_I2C_enableAnalogGlitchFilter(I2C_IMU_INST);




}

static const DL_UART_Main_ClockConfig gUART_ESP32ClockConfig = {
    .clockSel    = DL_UART_MAIN_CLOCK_BUSCLK,
    .divideRatio = DL_UART_MAIN_CLOCK_DIVIDE_RATIO_1
};

static const DL_UART_Main_Config gUART_ESP32Config = {
    .mode        = DL_UART_MAIN_MODE_NORMAL,
    .direction   = DL_UART_MAIN_DIRECTION_TX_RX,
    .flowControl = DL_UART_MAIN_FLOW_CONTROL_NONE,
    .parity      = DL_UART_MAIN_PARITY_NONE,
    .wordLength  = DL_UART_MAIN_WORD_LENGTH_8_BITS,
    .stopBits    = DL_UART_MAIN_STOP_BITS_ONE
};

SYSCONFIG_WEAK void SYSCFG_DL_UART_ESP32_init(void)
{
    DL_UART_Main_setClockConfig(UART_ESP32_INST, (DL_UART_Main_ClockConfig *) &gUART_ESP32ClockConfig);

    DL_UART_Main_init(UART_ESP32_INST, (DL_UART_Main_Config *) &gUART_ESP32Config);
    /*
     * Configure baud rate by setting oversampling and baud rate divisors.
     *  Target baud rate: 9600
     *  Actual baud rate: 9600.24
     */
    DL_UART_Main_setOversampling(UART_ESP32_INST, DL_UART_OVERSAMPLING_RATE_16X);
    DL_UART_Main_setBaudRateDivisor(UART_ESP32_INST, UART_ESP32_IBRD_32_MHZ_9600_BAUD, UART_ESP32_FBRD_32_MHZ_9600_BAUD);



    DL_UART_Main_enable(UART_ESP32_INST);
}

static const DL_SPI_Config gSPI_FRAM_config = {
    .mode        = DL_SPI_MODE_CONTROLLER,
    .frameFormat = DL_SPI_FRAME_FORMAT_MOTO4_POL0_PHA0,
    .parity      = DL_SPI_PARITY_NONE,
    .dataSize    = DL_SPI_DATA_SIZE_8,
    .bitOrder    = DL_SPI_BIT_ORDER_MSB_FIRST,
    .chipSelectPin = DL_SPI_CHIP_SELECT_0,
};

static const DL_SPI_ClockConfig gSPI_FRAM_clockConfig = {
    .clockSel    = DL_SPI_CLOCK_BUSCLK,
    .divideRatio = DL_SPI_CLOCK_DIVIDE_RATIO_1
};

SYSCONFIG_WEAK void SYSCFG_DL_SPI_FRAM_init(void) {
    DL_SPI_setClockConfig(SPI_FRAM_INST, (DL_SPI_ClockConfig *) &gSPI_FRAM_clockConfig);

    DL_SPI_init(SPI_FRAM_INST, (DL_SPI_Config *) &gSPI_FRAM_config);

    /* Configure Controller mode */
    /*
     * Set the bit rate clock divider to generate the serial output clock
     *     outputBitRate = (spiInputClock) / ((1 + SCR) * 2)
     *     8000000 = (32000000)/((1 + 1) * 2)
     */
    DL_SPI_setBitRateSerialClockDivider(SPI_FRAM_INST, 1);
    /* Set RX and TX FIFO threshold levels */
    DL_SPI_setFIFOThreshold(SPI_FRAM_INST, DL_SPI_RX_FIFO_LEVEL_1_2_FULL, DL_SPI_TX_FIFO_LEVEL_1_2_EMPTY);

    /* Enable module */
    DL_SPI_enable(SPI_FRAM_INST);
}

/* V_MEAS Initialization */
static const DL_ADC12_ClockConfig gV_MEASClockConfig = {
    .clockSel       = DL_ADC12_CLOCK_SYSOSC,
    .divideRatio    = DL_ADC12_CLOCK_DIVIDE_1,
    .freqRange      = DL_ADC12_CLOCK_FREQ_RANGE_24_TO_32,
};
SYSCONFIG_WEAK void SYSCFG_DL_V_MEAS_init(void)
{
    DL_ADC12_setClockConfig(V_MEAS_INST, (DL_ADC12_ClockConfig *) &gV_MEASClockConfig);
    DL_ADC12_configConversionMem(V_MEAS_INST, V_MEAS_ADCMEM_0,
        DL_ADC12_INPUT_CHAN_0, DL_ADC12_REFERENCE_VOLTAGE_INTREF, DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
        DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
    DL_ADC12_enableConversions(V_MEAS_INST);
}


static const DL_VREF_Config gVREFConfig = {
    .vrefEnable     = DL_VREF_ENABLE_ENABLE,
    .bufConfig      = DL_VREF_BUFCONFIG_OUTPUT_2_5V,
    .shModeEnable   = DL_VREF_SHMODE_DISABLE,
    .holdCycleCount = DL_VREF_HOLD_MIN,
    .shCycleCount   = DL_VREF_SH_MIN,
};

SYSCONFIG_WEAK void SYSCFG_DL_VREF_init(void) {
    DL_VREF_configReference(VREF,
        (DL_VREF_Config *) &gVREFConfig);
}


