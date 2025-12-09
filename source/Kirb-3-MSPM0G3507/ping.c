#include "ping.h"
#include "kirb-3.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include "uart_debug.h"

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
            .enabled = false,
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

// Ultrasonic Echo Pulse Width Captures in Microseconds
volatile uint16_t g_ult_pw_us[ULT_COUNT];

// Scheduler index: which sensor will be pinged next
volatile uint8_t g_ult_idx = 7;

// Latched index: which sensor this *current* ping/echo belongs to
volatile uint8_t g_ult_active_idx = 7;

// Float conversions for formatted display
float ping_us_to_cm_float(float pulse_width_us)
{
    return pulse_width_us / (float)PING_US_PER_CM;
}

float ping_us_to_in_float(float pulse_width_us)
{
    return pulse_width_us / (float)PING_US_PER_IN;
}

float ping_us_to_m_float(float pulse_width_us)
{
    // Convert µs → cm → meters
    float cm = ping_us_to_cm_float((uint32_t)pulse_width_us);
    return cm / (float)CM_PER_METER;
}

float ping_us_to_ft_float(float pulse_width_us)
{
    // Convert µs → inches → feet
    float inches = ping_us_to_in_float((uint32_t)pulse_width_us);
    return inches / (float)IN_PER_FEET;
}

void ping_init(void)
{
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
        // You can keep this extra belt-and-suspenders disable if you like;
        // echo IRQ also disables the timer on capture.
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
            // Optional: mark invalid
            // g_ult_pw_us[g_ult_active_idx] = 0xFFFF;
            return;
        }

        // Use the *latched* sensor index for this ping
        g_ult_pw_us[g_ult_active_idx] = (uint16_t)pulse_width_us;

        // (Optional debug)
        // UART_printf("echo: active=%u sched=%u pw=%lu\r\n",
        //             g_ult_active_idx, g_ult_idx, pulse_width_us);

        break;
    }

    default:
        return;
    }
}
