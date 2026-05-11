#ifndef RC_H
#define RC_H

/** @file rc.h
 *  @brief RC receiver pulse capture and validation interface.
 */

#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include <stdint.h>

/** Initialize cached RC channel pulse widths and GPIO edge state. */
void rc_init(void);

/** Handle capture interrupts for RC channels 1 through 3. */
void rc_ch1_3_irq(void);

/** Handle the capture interrupt for RC channel 4. */
void rc_ch4_irq(void);

/** Handle GPIO edge interrupts for RC channels 5 and 6. */
void rc_ch5_6_irq(void);

// -----------------------------------------------------------------------------
// RC Channel Indices and Logical Aliases
// -----------------------------------------------------------------------------
#define NON_APPLICABLE ((uint32_t)0xFFFFFFFFu)

#define RC_PULSE_WIDTH_MIN_VALID_US (800u)
#define RC_PULSE_WIDTH_NEUTRAL_US (1500u)
#define RC_PULSE_WIDTH_MAX_VALID_US (2200u)

/** Latest validated pulse width for each RC channel, in microseconds. */
extern volatile uint16_t g_rc_pw_us[];

typedef enum
{
    RC_CH1_IDX,
    RC_CH2_IDX,
    RC_CH3_IDX,
    RC_CH4_IDX,
    RC_CH5_IDX,
    RC_CH6_IDX,

    RC_CH_COUNT,
} RcIndex;

// Logical control mappings (FS-16 style)
#define RC_CH_LS_X (RC_CH1_IDX)
#define RC_CH_RS_Y (RC_CH2_IDX)
#define RC_CH_LS_Y (RC_CH3_IDX)
#define RC_CH_RS_X (RC_CH4_IDX)
#define RC_CH_VR_A (RC_CH5_IDX)
#define RC_CH_VR_B (RC_CH6_IDX)

typedef enum
{
    RC_BACKEND_TIMER,
    RC_BACKEND_GPIO,
} RcBackendType;

/** Static pin/timer mapping for one RC input channel. */
typedef struct
{
    RcBackendType backend;
    GPIO_Regs *gpio_port;
    uint32_t gpio_pin;
    uint32_t iomux_pincm;
    uint32_t iomux_func;
    void *timer_inst;
    uint32_t timer_cc;
    uint32_t irq_event;
} RcChannelConfig;

extern const RcChannelConfig g_rc_cfg[RC_CH_COUNT];

/** Return true when a pulse width is inside the accepted RC range. */
static inline bool rc_pulse_width_is_valid(uint16_t pulse_width_us)
{
    return (pulse_width_us >= RC_PULSE_WIDTH_MIN_VALID_US) &&
           (pulse_width_us <= RC_PULSE_WIDTH_MAX_VALID_US);
}

/** Replace invalid RC pulse widths with the neutral value. */
static inline uint16_t rc_filter_pulse_width_us(uint16_t pulse_width_us)
{
    if (!rc_pulse_width_is_valid(pulse_width_us))
    {
        return RC_PULSE_WIDTH_NEUTRAL_US;
    }

    return pulse_width_us;
}

#endif // RC_H
