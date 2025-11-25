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




/* Defines for QEI_0 */
#define QEI_0_INST                                                         TIMG8
#define QEI_0_INST_IRQHandler                                   TIMG8_IRQHandler
#define QEI_0_INST_INT_IRQN                                     (TIMG8_INT_IRQn)
/* Pin configuration defines for QEI_0 PHA Pin */
#define GPIO_QEI_0_PHA_PORT                                                GPIOB
#define GPIO_QEI_0_PHA_PIN                                         DL_GPIO_PIN_6
#define GPIO_QEI_0_PHA_IOMUX                                     (IOMUX_PINCM23)
#define GPIO_QEI_0_PHA_IOMUX_FUNC                    IOMUX_PINCM23_PF_TIMG8_CCP0
/* Pin configuration defines for QEI_0 PHB Pin */
#define GPIO_QEI_0_PHB_PORT                                                GPIOB
#define GPIO_QEI_0_PHB_PIN                                        DL_GPIO_PIN_16
#define GPIO_QEI_0_PHB_IOMUX                                     (IOMUX_PINCM33)
#define GPIO_QEI_0_PHB_IOMUX_FUNC                    IOMUX_PINCM33_PF_TIMG8_CCP1


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

/* Defines for ECHO_TIM */
#define ECHO_TIM_INST                                                    (TIMG0)
#define ECHO_TIM_INST_IRQHandler                                TIMG0_IRQHandler
#define ECHO_TIM_INST_INT_IRQN                                  (TIMG0_INT_IRQn)
#define ECHO_TIM_INST_LOAD_VALUE                                            (0U)
/* GPIO defines for channel 0 */
#define GPIO_ECHO_TIM_C0_PORT                                              GPIOA
#define GPIO_ECHO_TIM_C0_PIN                                      DL_GPIO_PIN_12
#define GPIO_ECHO_TIM_C0_IOMUX                                   (IOMUX_PINCM34)
#define GPIO_ECHO_TIM_C0_IOMUX_FUNC                  IOMUX_PINCM34_PF_TIMG0_CCP0
/* GPIO defines for channel 0 */
#define GPIO_ECHO_TIM_C0_PORT                                              GPIOA
#define GPIO_ECHO_TIM_C0_PIN                                      DL_GPIO_PIN_12
#define GPIO_ECHO_TIM_C0_IOMUX                                   (IOMUX_PINCM34)
#define GPIO_ECHO_TIM_C0_IOMUX_FUNC                  IOMUX_PINCM34_PF_TIMG0_CCP0





/* Defines for PING_SCHED_TIM */
#define PING_SCHED_TIM_INST                                              (TIMG6)
#define PING_SCHED_TIM_INST_IRQHandler                          TIMG6_IRQHandler
#define PING_SCHED_TIM_INST_INT_IRQN                            (TIMG6_INT_IRQn)
#define PING_SCHED_TIM_INST_LOAD_VALUE                                  (19999U)




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
/* Defines for BUF_EN: GPIOB.17 with pinCMx 43 on package pin 14 */
#define PING_BUF_EN_PORT                                                 (GPIOB)
#define PING_BUF_EN_PIN                                         (DL_GPIO_PIN_17)
#define PING_BUF_EN_IOMUX                                        (IOMUX_PINCM43)
/* Defines for BUF_DIR: GPIOB.12 with pinCMx 29 on package pin 64 */
#define PING_BUF_DIR_PORT                                                (GPIOB)
#define PING_BUF_DIR_PIN                                        (DL_GPIO_PIN_12)
#define PING_BUF_DIR_IOMUX                                       (IOMUX_PINCM29)
/* Defines for MUX0_EN: GPIOA.17 with pinCMx 39 on package pin 10 */
#define PING_MUX0_EN_PORT                                                (GPIOA)
#define PING_MUX0_EN_PIN                                        (DL_GPIO_PIN_17)
#define PING_MUX0_EN_IOMUX                                       (IOMUX_PINCM39)
/* Defines for MUX1_EN: GPIOA.16 with pinCMx 38 on package pin 9 */
#define PING_MUX1_EN_PORT                                                (GPIOA)
#define PING_MUX1_EN_PIN                                        (DL_GPIO_PIN_16)
#define PING_MUX1_EN_IOMUX                                       (IOMUX_PINCM38)
/* Defines for MUX_SEL0: GPIOB.18 with pinCMx 44 on package pin 15 */
#define PING_MUX_SEL0_PORT                                               (GPIOB)
#define PING_MUX_SEL0_PIN                                       (DL_GPIO_PIN_18)
#define PING_MUX_SEL0_IOMUX                                      (IOMUX_PINCM44)
/* Defines for MUX_SEL1: GPIOA.18 with pinCMx 40 on package pin 11 */
#define PING_MUX_SEL1_PORT                                               (GPIOA)
#define PING_MUX_SEL1_PIN                                       (DL_GPIO_PIN_18)
#define PING_MUX_SEL1_IOMUX                                      (IOMUX_PINCM40)
/* Defines for MUX_SEL2: GPIOA.24 with pinCMx 54 on package pin 25 */
#define PING_MUX_SEL2_PORT                                               (GPIOA)
#define PING_MUX_SEL2_PIN                                       (DL_GPIO_PIN_24)
#define PING_MUX_SEL2_IOMUX                                      (IOMUX_PINCM54)
/* Defines for PING_BUS: GPIOA.12 with pinCMx 34 on package pin 5 */
#define PING_PING_BUS_PORT                                               (GPIOA)
#define PING_PING_BUS_PIN                                       (DL_GPIO_PIN_12)
#define PING_PING_BUS_IOMUX                                      (IOMUX_PINCM34)
/* Port definition for Pin Group QEI */
#define QEI_PORT                                                         (GPIOB)

/* Defines for QEI1_A: GPIOB.7 with pinCMx 24 on package pin 59 */
#define QEI_QEI1_A_PIN                                           (DL_GPIO_PIN_7)
#define QEI_QEI1_A_IOMUX                                         (IOMUX_PINCM24)
/* Defines for QEI1_B: GPIOB.0 with pinCMx 12 on package pin 47 */
#define QEI_QEI1_B_PIN                                           (DL_GPIO_PIN_0)
#define QEI_QEI1_B_IOMUX                                         (IOMUX_PINCM12)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_MOTOR_PWM_init(void);
void SYSCFG_DL_QEI_0_init(void);
void SYSCFG_DL_RC_TIM0_init(void);
void SYSCFG_DL_RC_TIM1_init(void);
void SYSCFG_DL_ECHO_TIM_init(void);
void SYSCFG_DL_PING_SCHED_TIM_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
