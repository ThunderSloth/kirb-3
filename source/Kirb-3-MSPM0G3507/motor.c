#include "motor.h"

/** @file motor.c
 *  @brief Storage and initialization for PWM output state.
 */

volatile uint16_t g_mtr_pw_us[MOTOR_COUNT];
volatile uint16_t g_servo_pw_us[SERVO_COUNT];

/** Reset cached motor and servo output pulse widths to neutral. */
void motor_init(void)
{
    for (MtrIndex idx = 0; idx < MOTOR_COUNT; idx++)
    {
        g_mtr_pw_us[idx] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    }

    for (ServoIndex idx = 0; idx < SERVO_COUNT; idx++)
    {
        g_servo_pw_us[idx] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    }
}
