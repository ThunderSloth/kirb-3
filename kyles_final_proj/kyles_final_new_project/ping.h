#ifndef PING_H
#define PING_H

#include "kyles_new_kirb-3.h"
#include <stdint.h>

// -----------------------------------------------------------------------------
// Constants for PING))) distance conversion
// -----------------------------------------------------------------------------
//
// Standard approximations derived from speed of sound (~340 m/s):
//
//   distance_cm = pulse_width_us / 58
//   distance_in = pulse_width_us / 148
//
// Integer versions use straight division (truncate), while float versions
// give more precise display output.
//

#define PING_US_PER_CM 58  // microseconds per centimeter
#define PING_US_PER_IN 148 // microseconds per inch

#define IN_PER_FEET (12)
#define CM_PER_METER (100)

#define ULT_ECHO_MIN_US 150
#define ULT_ECHO_MAX_US 20000

#define ULT_INIT_PW (-1)

typedef enum
{
    ULT0_IDX = 0,
    ULT1_IDX,
    ULT2_IDX,
    ULT3_IDX,
    ULT4_IDX,
    ULT5_IDX,
    ULT6_IDX,
    ULT7_IDX,
    ULT8_IDX,
    ULT9_IDX,

    ULT_COUNT,
} UltIndex;

#define ANT_L2_ULT_IDX (ULT9_IDX)
#define ANT_L1_ULT_IDX (ULT8_IDX)
#define ANT_CTR_ULT_IDX (ULT7_IDX)
#define ANT_R1_ULT_IDX (ULT6_IDX)
#define ANT_R2_ULT_IDX (ULT5_IDX)

#define POST_R2_ULT_IDX (ULT4_IDX)
#define POST_R1_ULT_IDX (ULT3_IDX)
#define POST_CTR_ULT_IDX (ULT2_IDX)
#define POST_L1_ULT_IDX (ULT1_IDX)
#define POST_L2_ULT_IDX (ULT0_IDX)

// FRONT (ANT)
#define ULT0_ANGLE_DEG (-71.6f)
#define ULT1_ANGLE_DEG (-35.8f)
#define ULT2_ANGLE_DEG (0.0f)
#define ULT3_ANGLE_DEG (35.8f)
#define ULT4_ANGLE_DEG (71.6f)

// REAR (POST)
#define ULT5_ANGLE_DEG (108.4f)
#define ULT6_ANGLE_DEG (144.2f)
#define ULT7_ANGLE_DEG (180.0f)
#define ULT8_ANGLE_DEG (215.8f)
#define ULT9_ANGLE_DEG (251.6f)

typedef struct
{
    float angle_deg;
    float angle_rad;
    bool enabled;
} UltSensor;

extern UltSensor g_ult[];

#define DEG_TO_RAD (3.14159265358979323846f / 180.0f)

#define ULTS_PER_MUX (5)
#define TRIG_PW_US (5)

typedef enum
{
    MUX1_IDX,
    MUX2_IDX,
    MUX_COUNT,
} MuxIndex;

typedef struct
{
    GPIO_Regs *gpio_port;
    uint32_t gpio_pin;
    uint32_t iomux_pincm;
} MuxEnConfig;

static const MuxEnConfig g_mux_en_cfg[MUX_COUNT] = {
    [MUX1_IDX] =
        {
            .gpio_port = MUX_EN0_PORT,
            .gpio_pin = MUX_EN0_PIN,
            .iomux_pincm = MUX_EN0_IOMUX,
        },
    [MUX2_IDX] =
        {
            .gpio_port = MUX_EN1_PORT,
            .gpio_pin = MUX_EN1_PIN,
            .iomux_pincm = MUX_EN1_IOMUX,
        },
};

#define MUX_SEL_COUNT (3)

typedef struct
{
    GPIO_Regs *gpio_port;
    uint32_t gpio_pin;
    uint32_t iomux_pincm;
} MuxSelConfig;

static const MuxSelConfig g_mux_sel_cfg[MUX_SEL_COUNT] = {
    {
        .gpio_port = MUX_SEL0_PORT,
        .gpio_pin = MUX_SEL0_PIN,
        .iomux_pincm = MUX_SEL0_IOMUX,
    },
    {
        .gpio_port = MUX_SEL1_PORT,
        .gpio_pin = MUX_SEL1_PIN,
        .iomux_pincm = MUX_SEL1_IOMUX,
    },
    {
        .gpio_port = MUX_SEL2_PORT,
        .gpio_pin = MUX_SEL2_PIN,
        .iomux_pincm = MUX_SEL2_IOMUX,
    },
};
// -----------------------------------------------------------------------------
// Integer Conversion (fast, MCU-friendly)
// -----------------------------------------------------------------------------

// Convert pulse width (µs) to whole centimeters.
static inline uint32_t ping_us_to_cm_int(uint32_t pulse_width_us)
{
    return pulse_width_us / PING_US_PER_CM;
}

// Convert pulse width (µs) to whole inches.
static inline uint32_t ping_us_to_in_int(uint32_t pulse_width_us)
{
    return pulse_width_us / PING_US_PER_IN;
}

// -----------------------------------------------------------------------------
// Float Conversion (for display)
// -----------------------------------------------------------------------------

float ping_us_to_cm_float(float pulse_width_us);
float ping_us_to_in_float(float pulse_width_us);
float ping_us_to_m_float(float pulse_width_us);
float ping_us_to_ft_float(float pulse_width_us);

void ping_init(void);

void ult_echo_irq(void);
void ult_sched_irq(void);

extern volatile uint16_t g_ult_pw_us[];

#endif // PING_H
