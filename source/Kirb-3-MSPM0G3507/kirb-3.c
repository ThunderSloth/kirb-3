#include "kirb-3.h"

#include "battery.h"
#include "cmd_shell.h"
#include "command_bridge.h"
#include "encoder.h"
#include "imu.h"
#include "motor.h"
#include "ping.h"
#include "rc.h"
#include "system_state.h"
#include "telemetry.h"
#include "uart.h"

/** @file kirb-3.c
 *  @brief MSPM0 application entry point and interrupt dispatch.
 */

/** Initialize all firmware modules, enable interrupts, and run services. */
int main(void)
{
    SYSCFG_DL_init();
    rc_init();
    motor_init();
    battery_init();
    imu_init();
    ping_init();
    encoder_init();
    system_state_init();
    cmd_shell_init();
    command_bridge_init();
    telemetry_init();

    NVIC_EnableIRQ(RC_TIM0_INST_INT_IRQN);
    NVIC_EnableIRQ(RC_TIM1_INST_INT_IRQN);
    NVIC_EnableIRQ(GPIO_MULTIPLE_GPIOB_INT_IRQN);
    NVIC_EnableIRQ(ULT_SCHED_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(ULT_ECHO_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(ENC_SCHED_TIM_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_ENCODER_INST_INT_IRQN);
    NVIC_EnableIRQ(UART_ESP32_INST_INT_IRQN);
    NVIC_EnableIRQ(V_MEAS_INST_INT_IRQN);

    cmd_shell_print_boot_msg();

    while (1)
    {
        cmd_shell_poll();
        command_bridge_service();

        update_motor();
        update_servo();

        battery_service();
        encoder_service();
        imu_service();
        telemetry_service();
    }
}

/** Periodic 1 kHz-class scheduler for slow background tasks. */
void SysTick_Handler(void)
{
    battery_schedule();
    telemetry_schedule();
}

/** Dispatch RC timer 0 capture interrupts for channels 1-3. */
void RC_TIM0_INST_IRQHandler(void)
{
    rc_ch1_3_irq();
}

/** Dispatch RC timer 1 capture interrupts for channel 4. */
void RC_TIM1_INST_IRQHandler(void)
{
    rc_ch4_irq();
}

/** Dispatch grouped GPIO interrupts for IMU DRDY and RC channels 5-6. */
void GROUP1_IRQHandler(void)
{
    imu_gpio_irq();
    rc_ch5_6_irq();
}

/** Dispatch ultrasonic scheduler timer events. */
void ULT_SCHED_TIM_INST_IRQHandler(void)
{
    ult_sched_irq();
}

/** Dispatch ultrasonic echo capture events. */
void ULT_ECHO_TIM_INST_IRQHandler(void)
{
    ult_echo_irq();
}

/** Schedule the next encoder poll from the encoder timer. */
void ENC_SCHED_TIM_INST_IRQHandler(void)
{
    DL_TimerG_clearInterruptStatus(ENC_SCHED_TIM_INST,
                                   DL_TIMERG_INTERRUPT_ZERO_EVENT);
    encoder_schedule();
}

/** Dispatch encoder UART DMA-complete events. */
void UART_ENCODER_INST_IRQHandler(void)
{
    uart_encoder_irq();
}

/** Dispatch ESP32 command UART receive events. */
void UART_ESP32_INST_IRQHandler(void)
{
    command_bridge_uart_irq();
}

/** Dispatch battery ADC completion events. */
void V_MEAS_INST_IRQHandler(void)
{
    battery_adc_irq();
}
