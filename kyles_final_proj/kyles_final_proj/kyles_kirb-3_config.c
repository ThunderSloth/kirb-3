//=============================================================================
// System Initialization Source File
//=============================================================================
// This module contains all runtime initialization routines executed during
// system bring-up. Its responsibilities include:
//
//   • Enabling and resetting hardware blocks
//   • Configuring GPIO routing and direction
//   • Establishing system clock settings
//   • Initializing timing resources used by motor control and RC input
//
// Each function in this file performs a focused portion of the startup
// sequence. No function assumes prior peripheral state, and all hardware is
// placed into a known, clean configuration before use.
//
//=============================================================================



//-----------------------------------------------------------------------------
// Project Includes
//-----------------------------------------------------------------------------
// Standard C headers and core device definitions used throughout the project.
#include <stdio.h>
#include <ti/devices/msp/msp.h>



//-----------------------------------------------------------------------------
// Hardware Configuration & Low-Level Access
//-----------------------------------------------------------------------------
// Hardware abstraction layers, peripheral configuration routines, and
// definitions mapping the application logic to the physical device.
#include "kyles_kirb-3.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "clock.h"
#include "LaunchPad.h"
#include "ti_msp_dl_config.h"
#include "kyles_kirb-3_config.h"


//-----------------------------------------------------------------------------
// Local configuration tables (for this module only)
//-----------------------------------------------------------------------------

static GPIO_Regs *const g_gpio_ports[] = {
    GPIOA,
    GPIOB,
};

static GPTIMER_Regs *const g_gptimers[] = {
    MOTOR_PWM_INST,
    RC_TIM0_INST,
    RC_TIM1_INST,
};

#define NUM_GPIO_PORTS   (sizeof(g_gpio_ports) / sizeof(g_gpio_ports[0]))
#define NUM_GPTIMERS     (sizeof(g_gptimers) / sizeof(g_gptimers[0]))



//-----------------------------------------------------------------------------
// Local helpers
//-----------------------------------------------------------------------------

// Common 1 MHz up-counter configuration for all GPTIMER instances used here.
// Assumes a 32 MHz system bus clock and uses CC0 to define
// zero/advance/load behavior.
static void gptimer_init_1mhz_upcounter(GPTIMER_Regs *timer,
                                        uint32_t prescaler,
                                        uint32_t period_us)
{
    // Select bus clock as source
    timer->CLKSEL =
        GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE |
        GPTIMER_CLKSEL_MFCLK_SEL_DISABLE |
        GPTIMER_CLKSEL_LFCLK_SEL_DISABLE;

    // No clock division
    timer->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_1;

    // Prescale 32 MHz → 1 MHz tick
    update_reg(&timer->COMMONREGS.CPS,
               prescaler,
               GPTIMER_CPS_PCNT_MASK);

    // Set period
    timer->COUNTERREGS.LOAD = period_us - TIMER_LOAD_MINUS_ONE;

    // Configure for periodic up-count mode
    timer->COUNTERREGS.CTRCTL =
        GPTIMER_CTRCTL_CVAE_ZEROVAL |
        GPTIMER_CTRCTL_REPEAT_REPEAT_1 |
        GPTIMER_CTRCTL_CM_UP;

    // Assign CC0 to define zero / advance / load conditions
    update_reg(&timer->COUNTERREGS.CTRCTL,
               GPTIMER_CTRCTL_CZC_CCCTL0_ZCOND |
               GPTIMER_CTRCTL_CAC_CCCTL0_ACOND |
               GPTIMER_CTRCTL_CLC_CCCTL0_LCOND,
               GPTIMER_CTRCTL_CZC_MASK |
               GPTIMER_CTRCTL_CAC_MASK |
               GPTIMER_CTRCTL_CLC_MASK);
}

typedef enum {
    GPTIMER_CC_IDX_0,
    GPTIMER_CC_IDX_1,
    GPTIMER_CC_IDX_2,
    GPTIMER_CC_IDX_3,
} GptimerCcIndex;

typedef enum {
    GPTIMER_INPUT_DIRECT,
    GPTIMER_INPUT_PAIR,
} GptimerInputMode;

// Configure CC0 as a "zero-on-rising-edge" channel, no capture stored.
// This is used as the timebase reset for RC pulse width measurement.
static void gptimer_config_zero_on_rise_cc0(GPTIMER_Regs    *timer,
                                            GptimerInputMode input_mode)
{
    volatile uint32_t *ccctl_reg =
        &timer->COUNTERREGS.CCCTL_01[0];
    volatile uint32_t *ifctl_reg =
        &timer->COUNTERREGS.IFCTL_01[0];

    // Capture mode, zero on rising edge, no capture stored
    update_reg(ccctl_reg,
               GPTIMER_CCCTL_01_COC_CAPTURE |
               GPTIMER_CCCTL_01_ZCOND_CC_TRIG_RISE |
               GPTIMER_CCCTL_01_ACOND_TIMCLK |
               GPTIMER_CCCTL_01_CCOND_NOCAPTURE,
               GPTIMER_CCCTL_01_COC_MASK |
               GPTIMER_CCCTL_01_ZCOND_MASK |
               GPTIMER_CCCTL_01_LCOND_MASK |
               GPTIMER_CCCTL_01_ACOND_MASK |
               GPTIMER_CCCTL_01_CCOND_MASK);

    uint32_t input_select =
        (input_mode == GPTIMER_INPUT_PAIR)
            ? GPTIMER_IFCTL_01_ISEL_CCPX_INPUT_PAIR
            : GPTIMER_IFCTL_01_ISEL_CCPX_INPUT;

    update_reg(ifctl_reg,
               GPTIMER_IFCTL_01_INV_NOINVERT |
               input_select,
               GPTIMER_IFCTL_01_INV_MASK |
               GPTIMER_IFCTL_01_ISEL_MASK);
}

// Configure a CC channel for falling-edge capture on the timer input.
//
//  - CC0/CC1 → CCCTL_01 / IFCTL_01
//  - CC2/CC3 → CCCTL_23 / IFCTL_23
//
// For CC2/CC3, the input is always direct; input_mode is ignored.
// Note: CC0 falling-edge capture is not used in this project, so
// GPTIMER_CC_IDX_0 is treated as a no-op for now.
static void gptimer_config_fall_capture(GPTIMER_Regs    *timer,
                                        GptimerCcIndex   cc_index,
                                        GptimerInputMode input_mode)
{
    if (cc_index == GPTIMER_CC_IDX_0) {
        // Not used in this project; reserved for future expansion.
        return;
    }

    if (cc_index == GPTIMER_CC_IDX_1) {
        // CC1 lives in CCCTL_01 / IFCTL_01 at index 1
        uint32_t reg_index = 1u;

        volatile uint32_t *ccctl_reg =
            &timer->COUNTERREGS.CCCTL_01[reg_index];
        volatile uint32_t *ifctl_reg =
            &timer->COUNTERREGS.IFCTL_01[reg_index];

        // Capture on falling edge, no zero effect
        update_reg(ccctl_reg,
                   GPTIMER_CCCTL_01_COC_CAPTURE |
                   GPTIMER_CCCTL_01_ZCOND_CC_TRIG_NO_EFFECT |
                   GPTIMER_CCCTL_01_ACOND_TIMCLK |
                   GPTIMER_CCCTL_01_CCOND_CC_TRIG_FALL,
                   GPTIMER_CCCTL_01_COC_MASK |
                   GPTIMER_CCCTL_01_ZCOND_MASK |
                   GPTIMER_CCCTL_01_LCOND_MASK |
                   GPTIMER_CCCTL_01_ACOND_MASK |
                   GPTIMER_CCCTL_01_CCOND_MASK);

        uint32_t input_select =
            (input_mode == GPTIMER_INPUT_PAIR)
                ? GPTIMER_IFCTL_01_ISEL_CCPX_INPUT_PAIR
                : GPTIMER_IFCTL_01_ISEL_CCPX_INPUT;

        update_reg(ifctl_reg,
                   GPTIMER_IFCTL_01_INV_NOINVERT |
                   input_select,
                   GPTIMER_IFCTL_01_INV_MASK |
                   GPTIMER_IFCTL_01_ISEL_MASK);
    } else {
        // CC2 / CC3 live in CCCTL_23 / IFCTL_23 at indices 0 / 1
        uint32_t reg_index = ((uint32_t)cc_index) - 2u;

        volatile uint32_t *ccctl_reg =
            &timer->COUNTERREGS.CCCTL_23[reg_index];
        volatile uint32_t *ifctl_reg =
            &timer->COUNTERREGS.IFCTL_23[reg_index];

        // Capture on falling edge, no zero effect
        update_reg(ccctl_reg,
                   GPTIMER_CCCTL_23_COC_CAPTURE |
                   GPTIMER_CCCTL_23_ZCOND_CC_TRIG_NO_EFFECT |
                   GPTIMER_CCCTL_23_ACOND_TIMCLK |
                   GPTIMER_CCCTL_23_CCOND_CC_TRIG_FALL,
                   GPTIMER_CCCTL_23_COC_MASK |
                   GPTIMER_CCCTL_23_ZCOND_MASK |
                   GPTIMER_CCCTL_23_LCOND_MASK |
                   GPTIMER_CCCTL_23_ACOND_MASK |
                   GPTIMER_CCCTL_23_CCOND_MASK);

        // Direct CCP input for CC2/CC3
        update_reg(ifctl_reg,
                   GPTIMER_IFCTL_23_INV_NOINVERT |
                   GPTIMER_IFCTL_23_ISEL_CCPX_INPUT,
                   GPTIMER_IFCTL_23_INV_MASK |
                   GPTIMER_IFCTL_23_ISEL_MASK);
    }
}



//=============================================================================
// Function Implementations
//=============================================================================
// All runtime logic begins below. Each function configures or drives a specific
// subsystem and should have a single, clear responsibility.



//-----------------------------------------------------------------------------
// config_init
//-----------------------------------------------------------------------------
// Performs the full system bring-up sequence. This function calls all
// subsystem-specific initialization routines:
//
//   - Power domain setup and peripheral resets
//   - Pin routing and GPIO configuration
//   - System clock configuration
//   - Timer and PWM setup
//   - RC capture timer configuration
//
// This is the top-level initialization entry point for the application.
//-----------------------------------------------------------------------------
void config_init(void)
{
    power_init();
    //gpio_init();
    SYSCFG_DL_GPIO_init();    
    sys_clock_init();
    pwm_init();
    rc_timer0_init();
    rc_timer1_init();
    echo_tim_init();
    trig_tim_init();
}



//-----------------------------------------------------------------------------
// power_init
//-----------------------------------------------------------------------------
// Establishes a known starting condition for all hardware used by the
// application. For each required peripheral, the routine:
//
//   1. Asserts a reset to clear sticky state and return registers to defaults
//   2. Enables the corresponding power domain
//   3. Waits briefly for clock and power conditions to stabilize
//
// No peripheral is assumed to be pre-configured; every block is explicitly
// reset and powered on before later modules configure it further.
//-----------------------------------------------------------------------------
void power_init(void)
{
    // Reset and enable all GPIO ports listed in g_gpio_ports
    for (uint32_t idx = 0; idx < NUM_GPIO_PORTS; idx++) {
        GPIO_Regs *gpio_port = g_gpio_ports[idx];

        gpio_port->GPRCM.RSTCTL =
            GPIO_RSTCTL_KEY_UNLOCK_W |
            GPIO_RSTCTL_RESETSTKYCLR_CLR |
            GPIO_RSTCTL_RESETASSERT_ASSERT;

        gpio_port->GPRCM.PWREN =
            GPIO_PWREN_KEY_UNLOCK_W |
            GPIO_PWREN_ENABLE_ENABLE;
    }

    // Reset and enable all GPTIMER instances used by this application
    for (uint32_t idx = 0; idx < NUM_GPTIMERS; idx++) {
        GPTIMER_Regs *timer = g_gptimers[idx];

        timer->GPRCM.RSTCTL =
            GPTIMER_RSTCTL_KEY_UNLOCK_W |
            GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
            GPTIMER_RSTCTL_RESETASSERT_ASSERT;

        timer->GPRCM.PWREN =
            GPTIMER_PWREN_KEY_UNLOCK_W |
            GPTIMER_PWREN_ENABLE_ENABLE;
    }

    // Allow peripherals time to come out of reset before configuration
    clock_delay(PWR_EN_DELAY_MS);
}



//-----------------------------------------------------------------------------
// gpio_init
//-----------------------------------------------------------------------------
// Configures all pins used by the application. Responsibilities include:
//
//   • Routing peripheral signals onto their assigned device pins
//   • Enabling output drivers for PWM channels
//   • Enabling input buffers for RC capture channels
//
// The function does not assume any prior pin state and only configures the
// pins required by higher-level modules.
//-----------------------------------------------------------------------------
void gpio_init(void)
{
    // Configure pins used for motor PWM output from motor config table
    for (uint32_t idx = 0; idx < MOTOR_COUNT; idx++) {
        const MtrConfig *motor_cfg = &g_mtr_cfg[idx];

        IOMUX->SECCFG.PINCM[motor_cfg->iomux_pincm] =
            motor_cfg->iomux_func |
            IOMUX_PINCM_PC_CONNECTED;

        motor_cfg->gpio_port->DOESET31_0 = motor_cfg->gpio_pin;
    }

    uint32_t rc_gpio_mask = 0u;

    // Configure RC channels based on backend type
    for (uint32_t idx = 0; idx < RC_CH_COUNT; idx++) {
        const RcChannelConfig *rc_cfg = &g_rc_cfg[idx];

        uint32_t base_input_cfg =
            IOMUX_PINCM_INENA_ENABLE  |
            IOMUX_PINCM_PC_CONNECTED  |
            IOMUX_PINCM_INV_DISABLE   |
            IOMUX_PINCM_PIPU_DISABLE  |
            IOMUX_PINCM_PIPD_DISABLE  |
            IOMUX_PINCM_HYSTEN_DISABLE |
            IOMUX_PINCM_WUEN_DISABLE;

        switch (rc_cfg->backend) {
            case RC_BACKEND_TIMER:
                // Timer-based capture: same pad electrical config, different PF
                IOMUX->SECCFG.PINCM[rc_cfg->iomux_pincm] =
                    base_input_cfg |
                    rc_cfg->iomux_func;      // e.g., TIMA0_CCPx
                break;

            case RC_BACKEND_GPIO:
                // GPIO-based capture: same pad config, PF = GPIO
                IOMUX->SECCFG.PINCM[rc_cfg->iomux_pincm] =
                    base_input_cfg |
                    IOMUX_PINCM_FUNCSEL_GPIO;

                rc_gpio_mask |= rc_cfg->irq_event;
                break;

            default:
                // No action for unknown backend (should not happen)
                break;
        }
    }

    // Clear interrupt statuses and enable interrupts for GPIO-backed RC pins
    if (rc_gpio_mask != 0u) {
        RC_IN_PORT->CPU_INT.ICLR  |= rc_gpio_mask;
        RC_IN_PORT->CPU_INT.IMASK |= rc_gpio_mask;
    }

    // Set interrupt polarity for GPIO pins (falling edge)
    RC_IN_PORT->POLARITY15_0  |= GPIO_POLARITY15_0_DIO13_FALL;
    RC_IN_PORT->POLARITY31_16 |= GPIO_POLARITY31_16_DIO20_FALL;
}



//-----------------------------------------------------------------------------
// sys_clock_init
//-----------------------------------------------------------------------------
// Sets up global clocking behavior for the system. This routine:
//
//   • Configures oscillator frequency and related timing parameters
//   • Enables optional timing sources used by other subsystems
//   • Programs clock dividers to establish CPU and peripheral timing
//
// These settings define the timing base for all timer modules and must be
// applied before any peripheral that depends on system clocking.
//-----------------------------------------------------------------------------
void sys_clock_init(void)
{
    // Set brown-out reset threshold to minimum sensitivity
    SYSCTL->SOCLOCK.BORTHRESHOLD = SYSCTL_BORTHRESHOLD_LEVEL_BORMIN;

    // Configure system oscillator to 32 MHz base frequency
    update_reg(&SYSCTL->SOCLOCK.SYSOSCCFG,
               SYSCTL_SYSOSCCFG_FREQ_SYSOSCBASE,
               SYSCTL_SYSOSCCFG_FREQ_MASK);

    // Enable medium-frequency tick (4 MHz time base)
    SYSCTL->SOCLOCK.MCLKCFG |= SYSCTL_MCLKCFG_USEMFTICK_ENABLE;

    // No division on the ultra-low-power clock
    update_reg(&SYSCTL->SOCLOCK.MCLKCFG,
               SYSCTL_MCLKCFG_UDIV_NODIVIDE,
               SYSCTL_MCLKCFG_UDIV_MASK);

    // Disable main MCLK divider (run at full SYSOSC rate)
    update_reg(&SYSCTL->SOCLOCK.MCLKCFG,
               DL_SYSCTL_MCLK_DIVIDER_DISABLE,
               SYSCTL_MCLKCFG_MDIV_MASK);
}



//-----------------------------------------------------------------------------
// pwm_init
//-----------------------------------------------------------------------------
// Initializes the timing resource used to generate motor PWM signals. The
// routine configures:
//
//   • Clock source and prescaler for the timing module
//   • Counting mode and load period for PWM timing
//   • Compare values defining pulse width
//   • Output action rules (set on zero, clear on compare, etc.)
//   • Pin output behavior including polarity and enable state
//
// The result is a fully configured PWM generator producing defined pulse
// widths on the assigned output channels.
//-----------------------------------------------------------------------------
void pwm_init(void)
{
    // Common 1 MHz up-counter configuration for the PWM timer
    gptimer_init_1mhz_upcounter(MOTOR_PWM_INST,
                                MOTOR_PWM_PRESCALER,
                                MOTOR_PWM_PERIOD_US);

    // Configure shadow compare for both PWM channels
    update_reg(&MOTOR_PWM_INST->COUNTERREGS.CCCTL_01[0],
               GPTIMER_CCCTL_01_CCUPD_ZERO_EVT,
               GPTIMER_CCCTL_01_CCUPD_MASK);
    update_reg(&MOTOR_PWM_INST->COUNTERREGS.CCCTL_01[1],
               GPTIMER_CCCTL_01_CCUPD_ZERO_EVT,
               GPTIMER_CCCTL_01_CCUPD_MASK);

    // Init duty cycle to neutral pulse width
    MOTOR_PWM_INST->COUNTERREGS.CC_01[0] = SERVO_NEUTRAL_PULSE_WIDTH_US;
    MOTOR_PWM_INST->COUNTERREGS.CC_01[1] = SERVO_NEUTRAL_PULSE_WIDTH_US;

    // Configure compare mode for CC registers
    update_reg(&MOTOR_PWM_INST->COUNTERREGS.CCCTL_01[0],
               GPTIMER_CCCTL_01_COC_COMPARE,
               GPTIMER_CCCTL_01_COC_MASK);
    update_reg(&MOTOR_PWM_INST->COUNTERREGS.CCCTL_01[1],
               GPTIMER_CCCTL_01_COC_COMPARE,
               GPTIMER_CCCTL_01_COC_MASK);

    // Configure CCP for output
    update_reg(&MOTOR_PWM_INST->COMMONREGS.CCPD,
               GPTIMER_CCPD_C0CCP0_OUTPUT |
               GPTIMER_CCPD_C0CCP1_OUTPUT,
               GPTIMER_CCPD_C0CCP0_MASK |
               GPTIMER_CCPD_C0CCP1_MASK);

    // Set CCP HIGH on zero and LOW on compare
    uint32_t ccact_pwm_cfg =
        GPTIMER_CCACT_01_FENACT_DISABLED |
        GPTIMER_CCACT_01_CC2UACT_DISABLED |
        GPTIMER_CCACT_01_CC2DACT_DISABLED |
        GPTIMER_CCACT_01_CUACT_CCP_LOW |
        GPTIMER_CCACT_01_CDACT_DISABLED |
        GPTIMER_CCACT_01_LACT_DISABLED |
        GPTIMER_CCACT_01_ZACT_CCP_HIGH;

    MOTOR_PWM_INST->COUNTERREGS.CCACT_01[0] = ccact_pwm_cfg;
    MOTOR_PWM_INST->COUNTERREGS.CCACT_01[1] = ccact_pwm_cfg;

    // Select signal generator output for CCP
    update_reg(&MOTOR_PWM_INST->COUNTERREGS.OCTL_01[0],
               GPTIMER_OCTL_01_CCPO_FUNCVAL,
               GPTIMER_OCTL_01_CCPO_MASK);

    // Enable output
    update_reg(&MOTOR_PWM_INST->COMMONREGS.ODIS,
               GPTIMER_ODIS_C0CCP0_CCP_OUTPUT_OCTL |
               (GPTIMER_ODIS_C0CCP1_CCP_OUTPUT_OCTL << GPTIMER_ODIS_C0CCP1_OFS),
               GPTIMER_ODIS_C0CCP0_MASK |
               GPTIMER_ODIS_C0CCP1_MASK);

    // Set polarity (active high, low while disabled)
    update_reg(&MOTOR_PWM_INST->COUNTERREGS.OCTL_01[0],
               GPTIMER_OCTL_01_CCPIV_LOW |
               GPTIMER_OCTL_01_CCPOINV_NOINV,
               GPTIMER_OCTL_01_CCPIV_MASK |
               GPTIMER_OCTL_01_CCPOINV_MASK);

    // Enable clock
    MOTOR_PWM_INST->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

    // Start timer
    MOTOR_PWM_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}



//-----------------------------------------------------------------------------
// rc_timer0_init
//-----------------------------------------------------------------------------
// Configures the timer used to capture RC pulse widths on three channels
// sharing a common time base.
//-----------------------------------------------------------------------------
void rc_timer0_init(void)
{
    // Common 1 MHz up-counter configuration for RC timer 0
    gptimer_init_1mhz_upcounter(RC_TIM0_INST,
                                RC_TIM_PRESCALER,
                                RC_TIM_PERIOD_US);

    // CC0: zero counter on rising edge, no capture stored, direct CCP input
    gptimer_config_zero_on_rise_cc0(RC_TIM0_INST,
                                    GPTIMER_INPUT_DIRECT);

    // CC1: capture on falling edge, input-pair mapped to CC0 pin
    gptimer_config_fall_capture(RC_TIM0_INST,
                                GPTIMER_CC_IDX_1,
                                GPTIMER_INPUT_PAIR);

    // CC2: capture on falling edge, direct input
    gptimer_config_fall_capture(RC_TIM0_INST,
                                GPTIMER_CC_IDX_2,
                                GPTIMER_INPUT_DIRECT);

    // CC3: capture on falling edge, direct input
    gptimer_config_fall_capture(RC_TIM0_INST,
                                GPTIMER_CC_IDX_3,
                                GPTIMER_INPUT_DIRECT);

    // Enable interrupts for the three capture channels
    RC_TIM0_INST->CPU_INT.IMASK |=
        GPTIMER_CPU_INT_IMASK_CCU1_SET |
        GPTIMER_CPU_INT_IMASK_CCU2_SET |
        GPTIMER_CPU_INT_IMASK_CCU3_SET;

    // Enable clock
    RC_TIM0_INST->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

    // Start timer
    RC_TIM0_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}



//-----------------------------------------------------------------------------
// rc_timer1_init
//-----------------------------------------------------------------------------
// Configures the timer used to capture RC pulse width on a single channel,
// using CC0 to zero the counter on rising edges and CC1 to capture the
// corresponding falling edges.
//-----------------------------------------------------------------------------
void rc_timer1_init(void)
{
    // Common 1 MHz up-counter configuration for RC timer 1
    gptimer_init_1mhz_upcounter(RC_TIM1_INST,
                                RC_TIM_PRESCALER,
                                RC_TIM_PERIOD_US);

    // CC0: zero counter on rising edge, no capture stored, direct CCP input
    gptimer_config_zero_on_rise_cc0(RC_TIM1_INST,
                                    GPTIMER_INPUT_DIRECT);

    // CC1: capture on falling edge, input-pair mapped to CC0 pin
    gptimer_config_fall_capture(RC_TIM1_INST,
                                GPTIMER_CC_IDX_1,
                                GPTIMER_INPUT_PAIR);

    // Enable interrupts for CC0 and CC1 capture events
    RC_TIM1_INST->CPU_INT.IMASK |=
        GPTIMER_CPU_INT_IMASK_CCU0_SET |
        GPTIMER_CPU_INT_IMASK_CCU1_SET;

    // Enable clock gate for the timer
    RC_TIM1_INST->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

    // Start timer (enable counter)
    RC_TIM1_INST->COUNTERREGS.CTRCTL |= GPTIMER_CTRCTL_EN_ENABLED;
}

//-----------------------------------------------------------------------------
// echo_tim_init
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
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

void echo_tim_init(void) {

    DL_TimerG_setClockConfig(ECHO_TIM_INST,
        (DL_TimerG_ClockConfig *) &gECHO_TIMClockConfig);

    DL_TimerG_initCaptureMode(ECHO_TIM_INST,
        (DL_TimerG_CaptureConfig *) &gECHO_TIMCaptureConfig);
    DL_TimerG_enableClock(ECHO_TIM_INST);

}

//-----------------------------------------------------------------------------
// trig_tim_init
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/*
 * Timer clock configuration to be sourced by BUSCLK /  (32000000 Hz)
 * timerClkFreq = (timerClkSrc / (timerClkDivRatio * (timerClkPrescale + 1)))
 *   1000000 Hz = 32000000 Hz / (1 * (31 + 1))
 */
static const DL_TimerG_ClockConfig gTRIG_TIMClockConfig = {
    .clockSel    = DL_TIMER_CLOCK_BUSCLK,
    .divideRatio = DL_TIMER_CLOCK_DIVIDE_1,
    .prescale    = 31U,
};

/*
 * Timer load value (where the counter starts from) is calculated as (timerPeriod * timerClockFreq) - 1
 * TRIG_TIM_INST_LOAD_VALUE = (20 ms * 1000000 Hz) - 1
 */
static const DL_TimerG_TimerConfig gTRIG_TIMTimerConfig = {
    .period     = TRIG_TIM_INST_LOAD_VALUE,
    .timerMode  = DL_TIMER_TIMER_MODE_PERIODIC_UP,
    .startTimer = DL_TIMER_STOP,
};

void trig_tim_init(void) {

    DL_TimerG_setClockConfig(TRIG_TIM_INST,
        (DL_TimerG_ClockConfig *) &gTRIG_TIMClockConfig);

    DL_TimerG_initTimerMode(TRIG_TIM_INST,
        (DL_TimerG_TimerConfig *) &gTRIG_TIMTimerConfig);
    DL_TimerG_enableInterrupt(TRIG_TIM_INST , DL_TIMERG_INTERRUPT_CC0_UP_EVENT |
		DL_TIMERG_INTERRUPT_LOAD_EVENT |
		DL_TIMERG_INTERRUPT_ZERO_EVENT);
    DL_TimerG_enableClock(TRIG_TIM_INST);

}