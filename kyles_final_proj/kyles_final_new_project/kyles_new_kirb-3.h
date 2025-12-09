#ifndef KIRB_H_
#define KIRB_H_

#include "ti_msp_dl_config.h"

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

// -----------------------------------------------------------------------------
// Function Initialization
// -----------------------------------------------------------------------------
void scale_motor_speed(void);
void set_drive_straight(void);
// =============================================================================

// GPIO functionality is always a pin function of 0x00000001
#define IOMUX_PINCM_FUNCSEL_GPIO (1u)

// SysTick initialization constants
#define MSPM0_CLOCK_FREQUENCY (CPUCLK_FREQ)
#define SYS_TICK_PERIOD (10.25E-3)
#define SYS_TICK_PERIOD_COUNT (SYS_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

#endif // KIRB_H_
