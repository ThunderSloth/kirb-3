#include "battery.h"

#include "kirb-3.h"

/** @file battery.c
 *  @brief Periodic ADC sampling and resistor-divider conversion for battery
 *         telemetry.
 */

/* Battery+ -- R_TOP -- ADC pin -- R_BOTTOM -- GND */
#define BATTERY_R_TOP_OHMS (47000u)
#define BATTERY_R_BOTTOM_OHMS (10000u)
#define BATTERY_ADC_MAX_COUNTS (4095u)
#define BATTERY_SAMPLE_PERIOD_TICKS (2u)

volatile uint16_t g_battery_mv = 0u;
volatile uint16_t g_battery_adc_raw = 0u;
volatile bool g_battery_result_ready = false;

static volatile bool g_battery_sample_due = false;
static volatile bool g_battery_conversion_active = false;
static volatile uint8_t g_battery_tick_count = 0u;

/** Convert the raw ADC code at the divider tap into pack millivolts. */
static uint16_t battery_adc_to_mv(uint16_t adc_raw)
{
    const uint32_t adc_mv =
        ((uint32_t)adc_raw * VREF_VOLTAGE_MV) / BATTERY_ADC_MAX_COUNTS;
    const uint32_t battery_mv =
        (adc_mv * (BATTERY_R_TOP_OHMS + BATTERY_R_BOTTOM_OHMS)) /
        BATTERY_R_BOTTOM_OHMS;

    if (battery_mv > 0xFFFFu)
    {
        return 0xFFFFu;
    }

    return (uint16_t)battery_mv;
}

/** Start one non-blocking ADC conversion for the battery divider channel. */
static void battery_start_conversion(void)
{
    DL_ADC12_enableConversions(V_MEAS_INST);
    DL_ADC12_startConversion(V_MEAS_INST);
    g_battery_conversion_active = true;
}

void battery_init(void)
{
    g_battery_mv = 0u;
    g_battery_adc_raw = 0u;
    g_battery_result_ready = false;
    g_battery_sample_due = true;
    g_battery_conversion_active = false;
    g_battery_tick_count = 0u;
}

/** Advance the SysTick-based divider that controls battery sample cadence. */
void battery_schedule(void)
{
    if (g_battery_tick_count < BATTERY_SAMPLE_PERIOD_TICKS)
    {
        g_battery_tick_count++;
    }

    if (g_battery_tick_count >= BATTERY_SAMPLE_PERIOD_TICKS)
    {
        g_battery_tick_count = 0u;
        g_battery_sample_due = true;
    }
}

/** Consume completed ADC results and start the next scheduled conversion. */
void battery_service(void)
{
    if (g_battery_result_ready)
    {
        g_battery_adc_raw = DL_ADC12_getMemResult(V_MEAS_INST, V_MEAS_ADCMEM_0);
        g_battery_mv = battery_adc_to_mv(g_battery_adc_raw);
        g_battery_result_ready = false;
        g_battery_conversion_active = false;
    }

    if (!g_battery_sample_due || g_battery_conversion_active)
    {
        return;
    }

    g_battery_sample_due = false;
    battery_start_conversion();
}

/** Mark ADC memory result availability from the ADC interrupt context. */
void battery_adc_irq(void)
{
    switch (DL_ADC12_getPendingInterrupt(V_MEAS_INST))
    {
    case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
        g_battery_result_ready = true;
        break;
    default:
        break;
    }
}
