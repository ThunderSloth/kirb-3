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
#define GPIO_MOTOR_PWM_C0_PIN                                      DL_GPIO_PIN_4
#define GPIO_MOTOR_PWM_C0_IOMUX                                  (IOMUX_PINCM17)
#define GPIO_MOTOR_PWM_C0_IOMUX_FUNC                 IOMUX_PINCM17_PF_TIMA1_CCP0
#define GPIO_MOTOR_PWM_C0_IDX                                DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_MOTOR_PWM_C1_PORT                                             GPIOB
#define GPIO_MOTOR_PWM_C1_PIN                                      DL_GPIO_PIN_1
#define GPIO_MOTOR_PWM_C1_IOMUX                                  (IOMUX_PINCM13)
#define GPIO_MOTOR_PWM_C1_IOMUX_FUNC                 IOMUX_PINCM13_PF_TIMA1_CCP1
#define GPIO_MOTOR_PWM_C1_IDX                                DL_TIMER_CC_1_INDEX




/* Defines for QEI_MOTOR1 */
#define QEI_MOTOR1_INST                                                    TIMG8
#define QEI_MOTOR1_INST_IRQHandler                              TIMG8_IRQHandler
#define QEI_MOTOR1_INST_INT_IRQN                                (TIMG8_INT_IRQn)
/* Pin configuration defines for QEI_MOTOR1 PHA Pin */
#define GPIO_QEI_MOTOR1_PHA_PORT                                           GPIOB
#define GPIO_QEI_MOTOR1_PHA_PIN                                    DL_GPIO_PIN_6
#define GPIO_QEI_MOTOR1_PHA_IOMUX                                (IOMUX_PINCM23)
#define GPIO_QEI_MOTOR1_PHA_IOMUX_FUNC               IOMUX_PINCM23_PF_TIMG8_CCP0
/* Pin configuration defines for QEI_MOTOR1 PHB Pin */
#define GPIO_QEI_MOTOR1_PHB_PORT                                           GPIOB
#define GPIO_QEI_MOTOR1_PHB_PIN                                   DL_GPIO_PIN_16
#define GPIO_QEI_MOTOR1_PHB_IOMUX                                (IOMUX_PINCM33)
#define GPIO_QEI_MOTOR1_PHB_IOMUX_FUNC               IOMUX_PINCM33_PF_TIMG8_CCP1


/* Defines for RC_TIM0 */
#define RC_TIM0_INST                                                     (TIMA0)
#define RC_TIM0_INST_IRQHandler                                 TIMA0_IRQHandler
#define RC_TIM0_INST_INT_IRQN                                   (TIMA0_INT_IRQn)
#define RC_TIM0_INST_LOAD_VALUE                                         (64999U)
/* GPIO defines for channel 0 */
#define GPIO_RC_TIM0_C0_PORT                                               GPIOB
#define GPIO_RC_TIM0_C0_PIN                                        DL_GPIO_PIN_8
#define GPIO_RC_TIM0_C0_IOMUX                                    (IOMUX_PINCM25)
#define GPIO_RC_TIM0_C0_IOMUX_FUNC                   IOMUX_PINCM25_PF_TIMA0_CCP0
/* GPIO defines for channel 0 */
#define GPIO_RC_TIM0_C0_PORT                                               GPIOB
#define GPIO_RC_TIM0_C0_PIN                                        DL_GPIO_PIN_8
#define GPIO_RC_TIM0_C0_IOMUX                                    (IOMUX_PINCM25)
#define GPIO_RC_TIM0_C0_IOMUX_FUNC                   IOMUX_PINCM25_PF_TIMA0_CCP0
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






/* Defines for I2C_IMU */
#define I2C_IMU_INST                                                        I2C1
#define I2C_IMU_INST_IRQHandler                                  I2C1_IRQHandler
#define I2C_IMU_INST_INT_IRQN                                      I2C1_INT_IRQn
#define GPIO_I2C_IMU_SDA_PORT                                              GPIOB
#define GPIO_I2C_IMU_SDA_PIN                                       DL_GPIO_PIN_3
#define GPIO_I2C_IMU_IOMUX_SDA                                   (IOMUX_PINCM16)
#define GPIO_I2C_IMU_IOMUX_SDA_FUNC                    IOMUX_PINCM16_PF_I2C1_SDA
#define GPIO_I2C_IMU_SCL_PORT                                              GPIOB
#define GPIO_I2C_IMU_SCL_PIN                                       DL_GPIO_PIN_2
#define GPIO_I2C_IMU_IOMUX_SCL                                   (IOMUX_PINCM15)
#define GPIO_I2C_IMU_IOMUX_SCL_FUNC                    IOMUX_PINCM15_PF_I2C1_SCL


/* Defines for UART_ESP32 */
#define UART_ESP32_INST                                                    UART1
#define UART_ESP32_INST_FREQUENCY                                       32000000
#define UART_ESP32_INST_IRQHandler                              UART1_IRQHandler
#define UART_ESP32_INST_INT_IRQN                                  UART1_INT_IRQn
#define GPIO_UART_ESP32_RX_PORT                                            GPIOA
#define GPIO_UART_ESP32_TX_PORT                                            GPIOA
#define GPIO_UART_ESP32_RX_PIN                                     DL_GPIO_PIN_9
#define GPIO_UART_ESP32_TX_PIN                                     DL_GPIO_PIN_8
#define GPIO_UART_ESP32_IOMUX_RX                                 (IOMUX_PINCM20)
#define GPIO_UART_ESP32_IOMUX_TX                                 (IOMUX_PINCM19)
#define GPIO_UART_ESP32_IOMUX_RX_FUNC                  IOMUX_PINCM20_PF_UART1_RX
#define GPIO_UART_ESP32_IOMUX_TX_FUNC                  IOMUX_PINCM19_PF_UART1_TX
#define UART_ESP32_BAUD_RATE                                              (9600)
#define UART_ESP32_IBRD_32_MHZ_9600_BAUD                                   (208)
#define UART_ESP32_FBRD_32_MHZ_9600_BAUD                                    (21)




/* Defines for SPI_FRAM */
#define SPI_FRAM_INST                                                      SPI1
#define SPI_FRAM_INST_IRQHandler                                SPI1_IRQHandler
#define SPI_FRAM_INST_INT_IRQN                                    SPI1_INT_IRQn
#define GPIO_SPI_FRAM_PICO_PORT                                           GPIOB
#define GPIO_SPI_FRAM_PICO_PIN                                   DL_GPIO_PIN_15
#define GPIO_SPI_FRAM_IOMUX_PICO                                (IOMUX_PINCM32)
#define GPIO_SPI_FRAM_IOMUX_PICO_FUNC                IOMUX_PINCM32_PF_SPI1_PICO
#define GPIO_SPI_FRAM_POCI_PORT                                           GPIOA
#define GPIO_SPI_FRAM_POCI_PIN                                   DL_GPIO_PIN_16
#define GPIO_SPI_FRAM_IOMUX_POCI                                (IOMUX_PINCM38)
#define GPIO_SPI_FRAM_IOMUX_POCI_FUNC                IOMUX_PINCM38_PF_SPI1_POCI
/* GPIO configuration for SPI_FRAM */
#define GPIO_SPI_FRAM_SCLK_PORT                                           GPIOA
#define GPIO_SPI_FRAM_SCLK_PIN                                   DL_GPIO_PIN_17
#define GPIO_SPI_FRAM_IOMUX_SCLK                                (IOMUX_PINCM39)
#define GPIO_SPI_FRAM_IOMUX_SCLK_FUNC                IOMUX_PINCM39_PF_SPI1_SCLK
#define GPIO_SPI_FRAM_CS0_PORT                                            GPIOA
#define GPIO_SPI_FRAM_CS0_PIN                                    DL_GPIO_PIN_26
#define GPIO_SPI_FRAM_IOMUX_CS0                                 (IOMUX_PINCM59)
#define GPIO_SPI_FRAM_IOMUX_CS0_FUNC                  IOMUX_PINCM59_PF_SPI1_CS0



/* Defines for V_MEAS */
#define V_MEAS_INST                                                         ADC0
#define V_MEAS_INST_IRQHandler                                   ADC0_IRQHandler
#define V_MEAS_INST_INT_IRQN                                     (ADC0_INT_IRQn)
#define V_MEAS_ADCMEM_0                                       DL_ADC12_MEM_IDX_0
#define V_MEAS_ADCMEM_0_REF                    DL_ADC12_REFERENCE_VOLTAGE_INTREF
#define V_MEAS_ADCMEM_0_REF_VOLTAGE_V                                       2.50
#define GPIO_V_MEAS_C0_PORT                                                GPIOA
#define GPIO_V_MEAS_C0_PIN                                        DL_GPIO_PIN_27
#define GPIO_V_MEAS_IOMUX_C0                                     (IOMUX_PINCM60)
#define GPIO_V_MEAS_IOMUX_C0_FUNC                 (IOMUX_PINCM60_PF_UNCONNECTED)


/* Defines for VREF */
#define VREF_VOLTAGE_MV                                                     2500




/* Port definition for Pin Group PING */
#define PING_PORT                                                        (GPIOA)

/* Defines for TRIG: GPIOA.12 with pinCMx 34 on package pin 5 */
#define PING_TRIG_PIN                                           (DL_GPIO_PIN_12)
#define PING_TRIG_IOMUX                                          (IOMUX_PINCM34)
/* Port definition for Pin Group RC_IN */
#define RC_IN_PORT                                                       (GPIOB)

/* Defines for CH5: GPIOB.17 with pinCMx 43 on package pin 14 */
// pins affected by this interrupt request:["CH5","CH6"]
#define RC_IN_INT_IRQN                                          (GPIOB_INT_IRQn)
#define RC_IN_INT_IIDX                          (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define RC_IN_CH5_IIDX                                      (DL_GPIO_IIDX_DIO17)
#define RC_IN_CH5_PIN                                           (DL_GPIO_PIN_17)
#define RC_IN_CH5_IOMUX                                          (IOMUX_PINCM43)
/* Defines for CH6: GPIOB.12 with pinCMx 29 on package pin 64 */
#define RC_IN_CH6_IIDX                                      (DL_GPIO_IIDX_DIO12)
#define RC_IN_CH6_PIN                                           (DL_GPIO_PIN_12)
#define RC_IN_CH6_IOMUX                                          (IOMUX_PINCM29)
/* Port definition for Pin Group QEI_MOTOR2 */
#define QEI_MOTOR2_PORT                                                  (GPIOB)

/* Defines for A: GPIOB.7 with pinCMx 24 on package pin 59 */
#define QEI_MOTOR2_A_PIN                                         (DL_GPIO_PIN_7)
#define QEI_MOTOR2_A_IOMUX                                       (IOMUX_PINCM24)
/* Defines for B: GPIOB.0 with pinCMx 12 on package pin 47 */
#define QEI_MOTOR2_B_PIN                                         (DL_GPIO_PIN_0)
#define QEI_MOTOR2_B_IOMUX                                       (IOMUX_PINCM12)
/* Defines for EN0: GPIOB.19 with pinCMx 45 on package pin 16 */
#define MUX_EN0_PORT                                                     (GPIOB)
#define MUX_EN0_PIN                                             (DL_GPIO_PIN_19)
#define MUX_EN0_IOMUX                                            (IOMUX_PINCM45)
/* Defines for EN1: GPIOA.22 with pinCMx 47 on package pin 18 */
#define MUX_EN1_PORT                                                     (GPIOA)
#define MUX_EN1_PIN                                             (DL_GPIO_PIN_22)
#define MUX_EN1_IOMUX                                            (IOMUX_PINCM47)
/* Defines for SEL0: GPIOB.18 with pinCMx 44 on package pin 15 */
#define MUX_SEL0_PORT                                                    (GPIOB)
#define MUX_SEL0_PIN                                            (DL_GPIO_PIN_18)
#define MUX_SEL0_IOMUX                                           (IOMUX_PINCM44)
/* Defines for SEL1: GPIOA.18 with pinCMx 40 on package pin 11 */
#define MUX_SEL1_PORT                                                    (GPIOA)
#define MUX_SEL1_PIN                                            (DL_GPIO_PIN_18)
#define MUX_SEL1_IOMUX                                           (IOMUX_PINCM40)
/* Defines for SEL2: GPIOA.24 with pinCMx 54 on package pin 25 */
#define MUX_SEL2_PORT                                                    (GPIOA)
#define MUX_SEL2_PIN                                            (DL_GPIO_PIN_24)
#define MUX_SEL2_IOMUX                                           (IOMUX_PINCM54)
/* Port definition for Pin Group BUF */
#define BUF_PORT                                                         (GPIOA)

/* Defines for EN: GPIOA.31 with pinCMx 6 on package pin 39 */
#define BUF_EN_PIN                                              (DL_GPIO_PIN_31)
#define BUF_EN_IOMUX                                              (IOMUX_PINCM6)
/* Defines for DIR: GPIOA.13 with pinCMx 35 on package pin 6 */
#define BUF_DIR_PIN                                             (DL_GPIO_PIN_13)
#define BUF_DIR_IOMUX                                            (IOMUX_PINCM35)


/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_MOTOR_PWM_init(void);
void SYSCFG_DL_QEI_MOTOR1_init(void);
void SYSCFG_DL_RC_TIM0_init(void);
void SYSCFG_DL_RC_TIM1_init(void);
void SYSCFG_DL_ECHO_TIM_init(void);
void SYSCFG_DL_I2C_IMU_init(void);
void SYSCFG_DL_UART_ESP32_init(void);
void SYSCFG_DL_SPI_FRAM_init(void);
void SYSCFG_DL_V_MEAS_init(void);
void SYSCFG_DL_VREF_init(void);


bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
