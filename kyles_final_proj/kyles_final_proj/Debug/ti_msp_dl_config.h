/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
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
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)


#define CPUCLK_FREQ                                                     32000000



/* Defines for MOTOR_PWM */
#define MOTOR_PWM_INST                                                     TIMA1
#define MOTOR_PWM_INST_IRQHandler                               TIMA1_IRQHandler
#define MOTOR_PWM_INST_INT_IRQN                                 (TIMA1_INT_IRQn)
#define MOTOR_PWM_INST_CLK_FREQ                                          1000000
/* GPIO defines for channel 0 */
#define GPIO_MOTOR_PWM_C0_PORT                                             GPIOB
#define GPIO_MOTOR_PWM_C0_PIN                                      DL_GPIO_PIN_2
#define GPIO_MOTOR_PWM_C0_IOMUX                                  (IOMUX_PINCM15)
#define GPIO_MOTOR_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM15_PF_TIMA1_CCP0
#define GPIO_MOTOR_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_MOTOR_PWM_C1_PORT                                             GPIOB
#define GPIO_MOTOR_PWM_C1_PIN                                      DL_GPIO_PIN_3
#define GPIO_MOTOR_PWM_C1_IOMUX                                  (IOMUX_PINCM16)
#define GPIO_MOTOR_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM16_PF_TIMA1_CCP1
#define GPIO_MOTOR_PWM_C1_IDX                                DL_TIMER_CC_1_INDEX



/* Defines for RC_TIM0 */
#define RC_TIM0_INST                                                     (TIMA0)
#define RC_TIM0_INST_IRQHandler                                 TIMA0_IRQHandler
#define RC_TIM0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define RC_TIM0_INST_LOAD_VALUE                                         (64999U)
/* GPIO defines for channel 0 */
#define GPIO_RC_TIM0_C0_PORT                                               GPIOA
#define GPIO_RC_TIM0_C0_PIN                                        DL_GPIO_PIN_8
#define GPIO_RC_TIM0_C0_IOMUX                                    (IOMUX_PINCM19)
#define GPIO_RC_TIM0_C0_IOMUX_FUNC                   IOMUX_PINCM19_PF_TIMA0_CCP0
/* GPIO defines for channel 0 */
#define GPIO_RC_TIM0_C0_PORT                                               GPIOA
#define GPIO_RC_TIM0_C0_PIN                                        DL_GPIO_PIN_8
#define GPIO_RC_TIM0_C0_IOMUX                                    (IOMUX_PINCM19)
#define GPIO_RC_TIM0_C0_IOMUX_FUNC                   IOMUX_PINCM19_PF_TIMA0_CCP0
/* GPIO defines for channel 2 */
#define GPIO_RC_TIM0_C2_PORT                                               GPIOA
#define GPIO_RC_TIM0_C2_PIN                                       DL_GPIO_PIN_15
#define GPIO_RC_TIM0_C2_IOMUX                                    (IOMUX_PINCM37)
#define GPIO_RC_TIM0_C2_IOMUX_FUNC                   IOMUX_PINCM37_PF_TIMA0_CCP2
/* GPIO defines for channel 3 */
#define GPIO_RC_TIM0_C3_PORT                                               GPIOA
#define GPIO_RC_TIM0_C3_PIN                                       DL_GPIO_PIN_25
#define GPIO_RC_TIM0_C3_IOMUX                                    (IOMUX_PINCM55)
#define GPIO_RC_TIM0_C3_IOMUX_FUNC                   IOMUX_PINCM55_PF_TIMA0_CCP3

/* Defines for RC_TIM1 */
#define RC_TIM1_INST                                                     (TIMG7)
#define RC_TIM1_INST_IRQHandler                                 TIMG7_IRQHandler
#define RC_TIM1_INST_INT_IRQN                                   (TIMG7_INT_IRQn)
#define RC_TIM1_INST_LOAD_VALUE                                         (64999U)
/* GPIO defines for channel 0 */
#define GPIO_RC_TIM1_C0_PORT                                               GPIOA
#define GPIO_RC_TIM1_C0_PIN                                       DL_GPIO_PIN_28
#define GPIO_RC_TIM1_C0_IOMUX                                     (IOMUX_PINCM3)
#define GPIO_RC_TIM1_C0_IOMUX_FUNC                    IOMUX_PINCM3_PF_TIMG7_CCP0
/* GPIO defines for channel 0 */
#define GPIO_RC_TIM1_C0_PORT                                               GPIOA
#define GPIO_RC_TIM1_C0_PIN                                       DL_GPIO_PIN_28
#define GPIO_RC_TIM1_C0_IOMUX                                     (IOMUX_PINCM3)
#define GPIO_RC_TIM1_C0_IOMUX_FUNC                    IOMUX_PINCM3_PF_TIMG7_CCP0






/* Port definition for Pin Group RC_IN */
#define RC_IN_PORT                                                       (GPIOB)

/* Defines for CH5: GPIOB.13 with pinCMx 30 on package pin 1 */
// pins affected by this interrupt request:["CH5","CH6"]
#define RC_IN_INT_IRQN                                          (GPIOB_INT_IRQn)
#define RC_IN_INT_IIDX                          (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define RC_IN_CH5_IIDX                                      (DL_GPIO_IIDX_DIO13)
#define RC_IN_CH5_PIN                                           (DL_GPIO_PIN_13)
#define RC_IN_CH5_IOMUX                                          (IOMUX_PINCM30)
/* Defines for CH6: GPIOB.20 with pinCMx 48 on package pin 19 */
#define RC_IN_CH6_IIDX                                      (DL_GPIO_IIDX_DIO20)
#define RC_IN_CH6_PIN                                           (DL_GPIO_PIN_20)
#define RC_IN_CH6_IOMUX                                          (IOMUX_PINCM48)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_MOTOR_PWM_init(void);
void SYSCFG_DL_RC_TIM0_init(void);
void SYSCFG_DL_RC_TIM1_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
