#ifndef RC_H
#define RC_H

#include "ti/driverlib/dl_timera.h"
#include "ti/driverlib/dl_timerg.h"
#include "ti_msp_dl_config.h"
#include <stdint.h>

// Standard hobby servo pulse widths (microseconds)
#define SERVO_MIN_PULSE_WIDTH_US (1000u)
#define SERVO_NEUTRAL_PULSE_WIDTH_US (1500u)
#define SERVO_MAX_PULSE_WIDTH_US (2000u)
#define SERVO_MIN_PULSE_WIDTH_DIFF_US (50u) // May need to be adjusted

void rc_ch1_3_irq(void);
void rc_ch4_irq(void);
void rc_ch5_6_irq(void);

// -----------------------------------------------------------------------------
// RC Channel Indices and Logical Aliases
// -----------------------------------------------------------------------------
#define NON_APPLICABLE ((uint32_t)0xFFFFFFFFu)

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

typedef struct
{
    RcBackendType backend; // TIMER or GPIO-based capture
    GPIO_Regs *gpio_port;  // Pin bank used for this channel
    uint32_t gpio_pin;     // Pin mask within that bank
    uint32_t iomux_pincm;  // IOMUX PINCM index for this pin
    uint32_t iomux_func;   // PF function value for this pin (or NON_APPLICABLE
                           // for plain GPIO)
    void *timer_inst;      // Timer instance used for capture/timebase
    uint32_t timer_cc;  // CC index for TIMER backend, NON_APPLICABLE for GPIO
    uint32_t irq_event; // TIMER: IIDX_CCx_UP, GPIO: pin mask for MIS/IMASK
} RcChannelConfig;

extern const RcChannelConfig g_rc_cfg[RC_CH_COUNT];

#endif // RC_H