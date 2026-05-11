#ifndef BATTERY_H
#define BATTERY_H

/** @file battery.h
 *  @brief Battery voltage sampling interface for the MSPM0 firmware.
 */

#include <stdbool.h>
#include <stdint.h>

/** Latest converted battery voltage, in millivolts. */
extern volatile uint16_t g_battery_mv;

/** Latest raw ADC conversion result for the battery divider. */
extern volatile uint16_t g_battery_adc_raw;

/** Set by the ADC ISR when a new battery sample is ready to consume. */
extern volatile bool g_battery_result_ready;

/** Initialize cached battery state and request the first sample. */
void battery_init(void);

/** Advance the battery sampling divider from the SysTick scheduler. */
void battery_schedule(void);

/** Start conversions and publish completed battery readings. */
void battery_service(void);

/** Handle the ADC memory-result interrupt for the battery measurement. */
void battery_adc_irq(void);

#endif // BATTERY_H
