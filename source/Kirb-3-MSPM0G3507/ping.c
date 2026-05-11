#include "ping.h"
#include "kirb-3.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"

/** @file ping.c
 *  @brief Time-multiplexed trigger/echo handling for ten ultrasonic sensors.
 */

UltSensor g_ult[ULT_COUNT] = {

    // -------------------- FRONT (ANT) --------------------
    [ULT0_IDX] =
        {
            .angle_deg = ULT0_ANGLE_DEG,
            .angle_rad = ULT0_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT1_IDX] =
        {
            .angle_deg = ULT1_ANGLE_DEG,
            .angle_rad = ULT1_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT2_IDX] =
        {
            .angle_deg = ULT2_ANGLE_DEG,
            .angle_rad = ULT2_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT3_IDX] =
        {
            .angle_deg = ULT3_ANGLE_DEG,
            .angle_rad = ULT3_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT4_IDX] =
        {
            .angle_deg = ULT4_ANGLE_DEG,
            .angle_rad = ULT4_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },

    // -------------------- REAR (POST) --------------------
    [ULT5_IDX] =
        {
            .angle_deg = ULT5_ANGLE_DEG,
            .angle_rad = ULT5_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT6_IDX] =
        {
            .angle_deg = ULT6_ANGLE_DEG,
            .angle_rad = ULT6_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT7_IDX] =
        {
            .angle_deg = ULT7_ANGLE_DEG,
            .angle_rad = ULT7_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT8_IDX] =
        {
            .angle_deg = ULT8_ANGLE_DEG,
            .angle_rad = ULT8_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
    [ULT9_IDX] =
        {
            .angle_deg = ULT9_ANGLE_DEG,
            .angle_rad = ULT9_ANGLE_DEG * DEG_TO_RAD,
            .enabled = true,
        },
};

volatile uint16_t g_ult_pw_us[ULT_COUNT];
static uint16_t g_ult_pw_history[ULT_COUNT][3];
static uint8_t g_ult_pw_history_count[ULT_COUNT];
static uint8_t g_ult_pw_history_head[ULT_COUNT];
static uint8_t g_ult_invalid_count[ULT_COUNT];

/** Scheduler index: which sensor will be pinged next. */
volatile uint8_t g_ult_idx = ULT0_IDX;

/** Latched index: which sensor this current ping/echo belongs to. */
volatile uint8_t g_ult_active_idx = ULT0_IDX;

/** Return the median value from three pulse-width samples. */
static uint16_t ult_median3(uint16_t a, uint16_t b, uint16_t c)
{
    if (a > b)
    {
        const uint16_t tmp = a;
        a = b;
        b = tmp;
    }

    if (b > c)
    {
        const uint16_t tmp = b;
        b = c;
        c = tmp;
    }

    if (a > b)
    {
        const uint16_t tmp = a;
        a = b;
        b = tmp;
    }

    return b;
}

/** Add a valid sample to the rolling median filter for one sensor. */
static uint16_t ult_filter_valid_sample(uint8_t idx, uint16_t pulse_width_us)
{
    uint8_t head = g_ult_pw_history_head[idx];
    uint8_t count = g_ult_pw_history_count[idx];

    g_ult_pw_history[idx][head] = pulse_width_us;
    head = (uint8_t)((head + 1u) % 3u);
    g_ult_pw_history_head[idx] = head;

    if (count < 3u)
    {
        count++;
        g_ult_pw_history_count[idx] = count;
    }

    if (count == 1u)
    {
        return g_ult_pw_history[idx][0];
    }

    if (count == 2u)
    {
        const uint16_t a = g_ult_pw_history[idx][0];
        const uint16_t b = g_ult_pw_history[idx][1];
        return (uint16_t)(((uint32_t)a + (uint32_t)b) / 2u);
    }

    return ult_median3(g_ult_pw_history[idx][0], g_ult_pw_history[idx][1],
                       g_ult_pw_history[idx][2]);
}

/** Convert ultrasonic echo pulse width to centimeters as float. */
float ping_us_to_cm_float(float pulse_width_us)
{
    return pulse_width_us / (float)PING_US_PER_CM;
}

/** Convert ultrasonic echo pulse width to inches as float. */
float ping_us_to_in_float(float pulse_width_us)
{
    return pulse_width_us / (float)PING_US_PER_IN;
}

/** Convert ultrasonic echo pulse width to meters as float. */
float ping_us_to_m_float(float pulse_width_us)
{
    // Convert µs → cm → meters
    float cm = ping_us_to_cm_float((uint32_t)pulse_width_us);
    return cm / (float)CM_PER_METER;
}

/** Convert ultrasonic echo pulse width to feet as float. */
float ping_us_to_ft_float(float pulse_width_us)
{
    // Convert µs → inches → feet
    float inches = ping_us_to_in_float((uint32_t)pulse_width_us);
    return inches / (float)IN_PER_FEET;
}

/** Initialize ultrasonic sensor state, mux routing, and schedule timer. */
void ping_init(void)
{
    for (uint8_t i = 0u; i < ULT_COUNT; i++)
    {
        g_ult_pw_us[i] = ULT_ECHO_MAX_US;
        g_ult_pw_history_count[i] = 0u;
        g_ult_pw_history_head[i] = 0u;
        g_ult_invalid_count[i] = 0u;
        for (uint8_t j = 0u; j < 3u; j++)
        {
            g_ult_pw_history[i][j] = ULT_ECHO_MAX_US;
        }
    }

    // Disable Ping IO output driver
    PING_PORT->DOECLR31_0 = PING_TRIG_PIN;
    // Route Trigger pin to GPIO output
    IOMUX->SECCFG.PINCM[PING_TRIG_IOMUX] =
        (IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_FUNCSEL_GPIO);

    // Enable buffer
    BUF_PORT->DOUTCLR31_0 = BUF_EN_PIN;

    // Set trigger pulse duration
    ULT_SCHED_TIM_INST->COUNTERREGS.CC_01[0] = TRIG_PW_US;
    // Start Ultrasonic Scheduler Timer
    ULT_SCHED_TIM_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}

/** Handle scheduler timer events for trigger, listen, and sensor advance. */
void ult_sched_irq(void)
{
    switch (DL_Timer_getPendingInterrupt(ULT_SCHED_TIM_INST))
    {
    case DL_TIMER_IIDX_ZERO:
    {
        // End previous ping and clear echo timer state
        ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);

        // Clear any pending CC1 interrupt from the last ping
        // (Use the correct CC1 interrupt mask for your device here)
        DL_Timer_clearInterruptStatus(ULT_ECHO_TIM_INST,
                                      DL_TIMER_INTERRUPT_CC1_UP_EVENT);

        // Disable Trigger output driver
        PING_PORT->DOECLR31_0 = PING_TRIG_PIN;

        // Disable both Mux branches
        for (uint8_t mux_idx = 0; mux_idx < MUX_COUNT; mux_idx++)
        {
            const MuxEnConfig *cfg = &g_mux_en_cfg[mux_idx];
            GPIO_Regs *port = cfg->gpio_port;
            uint32_t pin = cfg->gpio_pin;
            port->DOUTSET31_0 = pin;
        }

        // Latch the index for this ping so ISR can't see a "future" index
        g_ult_active_idx = g_ult_idx;

        // Determine branch and channel for *this* Sensor Index
        uint8_t branch = g_ult_active_idx / ULTS_PER_MUX;
        uint8_t channel = g_ult_active_idx % ULTS_PER_MUX;

        // Set Mux channel-select lines
        for (uint8_t sel_idx = 0; sel_idx < MUX_SEL_COUNT; sel_idx++)
        {
            const MuxSelConfig *cfg = &g_mux_sel_cfg[sel_idx];
            GPIO_Regs *port = cfg->gpio_port;
            uint32_t pin = cfg->gpio_pin;

            bool bit = (channel >> sel_idx) & 1;

            if (bit)
            {
                port->DOUTSET31_0 = pin;
            }
            else
            {
                port->DOUTCLR31_0 = pin;
            }
        }

        // Set Buffer direction to output (drive trigger towards sensor)
        BUF_PORT->DOUTSET31_0 = BUF_DIR_PIN;

        // Enable selected Mux branch
        const MuxEnConfig *mux = &g_mux_en_cfg[branch];
        GPIO_Regs *mux_port = mux->gpio_port;
        uint32_t mux_pin = mux->gpio_pin;
        mux_port->DOUTCLR31_0 = mux_pin;

        // Set Trigger output level HIGH and enable driver (start pulse)
        PING_PORT->DOUTSET31_0 = PING_TRIG_PIN;
        PING_PORT->DOESET31_0 = PING_TRIG_PIN;

        break;
    }

    case DL_TIMER_IIDX_CC0_UP:
    {
        // End trigger pulse
        PING_PORT->DOUTCLR31_0 = PING_TRIG_PIN;
        PING_PORT->DOECLR31_0 = PING_TRIG_PIN;

        // Set Buffer direction to input (listen for echo)
        BUF_PORT->DOUTCLR31_0 = BUF_DIR_PIN;

        // Enable Echo Timer for this ping
        ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;

        break;
    }

    case DL_TIMER_IIDX_LOAD:
    {

        ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);

        // Advance Scheduler Sensor Index to next enabled sensor
        bool done = false;
        while (!done)
        {
            if (++g_ult_idx >= ULT_COUNT)
            {
                g_ult_idx = 0;
            }
            if (g_ult[g_ult_idx].enabled)
            {
                done = true;
            }
        }

        break;
    }

    default:
        return;
    }
}

/** Capture and filter the active ultrasonic sensor's echo pulse width. */
void ult_echo_irq(void)
{
    switch (DL_Timer_getPendingInterrupt(ULT_ECHO_TIM_INST))
    {
    case DL_TIMER_IIDX_CC1_UP:
    {
        // Stop echo timer immediately
        ULT_ECHO_TIM_INST->COUNTERREGS.CTRCTL &= ~(GPTIMER_CTRCTL_EN_ENABLED);

        // Store CC1 capture as pulse width for current sensor
        uint32_t pulse_width_us = DL_Timer_getCaptureCompareValue(
            ULT_ECHO_TIM_INST, DL_TIMER_CC_1_INDEX);

        // Ignore glitches / out-of-range; timer already stopped
        if ((pulse_width_us <= ULT_ECHO_MIN_US) ||
            (pulse_width_us >= ULT_ECHO_MAX_US))
        {
            const uint8_t idx = g_ult_active_idx;

            if (g_ult_invalid_count[idx] < 0xFFu)
            {
                g_ult_invalid_count[idx]++;
            }

            if (g_ult_invalid_count[idx] > ULT_MAX_INVALID_HOLD)
            {
                g_ult_pw_us[idx] = ULT_ECHO_MAX_US;
                g_ult_pw_history_count[idx] = 0u;
                g_ult_pw_history_head[idx] = 0u;
                for (uint8_t j = 0u; j < 3u; j++)
                {
                    g_ult_pw_history[idx][j] = ULT_ECHO_MAX_US;
                }
            }

            return;
        }

        // Use the *latched* sensor index for this ping
        {
            const uint8_t idx = g_ult_active_idx;
            g_ult_invalid_count[idx] = 0u;
            g_ult_pw_us[idx] =
                ult_filter_valid_sample(idx, (uint16_t)pulse_width_us);
        }

        // (Optional debug)
        // UART_printf("echo: active=%u sched=%u pw=%lu\r\n",
        //             g_ult_active_idx, g_ult_idx, pulse_width_us);

        break;
    }

    default:
        return;
    }
}
