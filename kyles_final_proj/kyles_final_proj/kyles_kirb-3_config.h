#ifndef KIRB_CONFIG_H_
#define KIRB_CONFIG_H_

//=============================================================================
// Configuration Header
//=============================================================================
// This file declares initialization routines and helper utilities used to
// configure system clocks, I/O routing, timers, and power domains. It exposes
// symbolic configuration constants and small inline helpers needed across the
// application. No hardware-specific assumptions are documented here so that
// the implementation can evolve without invalidating this interface.
//=============================================================================


//-----------------------------------------------------------------------------
// Compile-Time Configuration Constants
//-----------------------------------------------------------------------------
// Static configuration parameters used during system bring-up.
// This section defines values the application depends on at build time.

/*
 * NOTE ON THE 'u' SUFFIX:
 * ------------------------
 * Some constants below use a trailing 'u' (e.g., 31u or 0xFFFFu).
 * This marks the literal as an *unsigned* value. It prevents subtle bugs
 * when these constants are used in bit-masks, register fields, or timer
 * calculations where signed math could behave unexpectedly.
 *
 * Regular integers (like delays or simple counts) don't need the suffix.
 * Hardware-related constants (prescalers, max counts, masks, etc.) should
 * use it to keep the compiler honest.
 */

#define PWR_EN_DELAY_MS         (24)

#define TIMER_LOAD_MINUS_ONE    (1u)

#define PRESCALER_32MHZ_TO_1US  (31u)    // Prescaler divisor (PCNT = N means divide by N+1)
#define MAX_PCOUNT_16BIT_TIM    (0xFFFFu)

#define MOTOR_PWM_PERIOD_US     (20000u)   // PWM period in microseconds
#define MOTOR_PWM_PRESCALER     (PRESCALER_32MHZ_TO_1US)

#define RC_TIM_PRESCALER        (PRESCALER_32MHZ_TO_1US)
#define RC_TIM_PERIOD_US        (MAX_PCOUNT_16BIT_TIM)

//-----------------------------------------------------------------------------
// Type Definitions
//-----------------------------------------------------------------------------
// Add typedefs or configuration structures here if future modules require them.


//-----------------------------------------------------------------------------
// Public Initialization Routines
//-----------------------------------------------------------------------------
// Each function configures one subsystem. Implementations appear in the
// corresponding .c file; this header only provides the interface.
// Functions are grouped by subsystem responsibility.

void config_init(void);
void power_init(void);
void gpio_init(void);
void sys_clock_init(void);
void pwm_init(void);
void rc_timer0_init(void);
void rc_timer1_init(void);
void echo_tim_init(void);
void trig_tim_init(void);


//-----------------------------------------------------------------------------
// Inline Utility Helpers
//-----------------------------------------------------------------------------
// Small, frequently used, type-safe helpers that manipulate registers or
// compute configuration values. These remain inline to avoid call overhead.

/*
 * NOTE ON STATIC INLINE FUNCTIONS vs MACROS:
 * ------------------------------------------
 * Older C code often uses macros (#define) for small utility operations,
 * but macros have no type-checking, no debugging visibility, and can
 * introduce surprising behavior due to textual substitution.
 *
 * Modern embedded C prefers 'static inline' functions instead:
 *    - 'inline' lets the compiler eliminate call overhead
 *    - 'static' keeps the function local to this file (no global symbol)
 *    - they behave like real functions with proper types and safety
 *
 * Macros are still useful for constant values and bit-masks, but
 * inline functions are safer for any operation that takes arguments.
 */
 
static inline void update_reg(volatile uint32_t *reg,
                             uint32_t value,
                             uint32_t mask)
{
    uint32_t temp = *reg;
    temp &= ~mask;
    temp |= (value & mask);
    *reg = temp;
}


//-----------------------------------------------------------------------------
// Global Variables (avoid if possible)
//-----------------------------------------------------------------------------
// Declare extern globals here only if absolutely required by multiple modules.


//-----------------------------------------------------------------------------
// End of Header
//-----------------------------------------------------------------------------
#endif  // KIRB_CONFIG_H_
