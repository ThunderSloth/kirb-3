#ifndef KIRB_H_
#define KIRB_H_

/** @file kirb-3.h
 *  @brief Shared MSPM0 board configuration includes and project constants.
 */

#include "ti_msp_dl_config.h"

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

/** GPIO functionality is always a pin function of 0x00000001. */
#define IOMUX_PINCM_FUNCSEL_GPIO (1u)

/** CPU clock used to derive SysTick constants. */
#define MSPM0_CLOCK_FREQUENCY (CPUCLK_FREQ)

/** Requested SysTick period, in seconds. */
#define SYST_TICK_PERIOD (10.25E-3)

/** SysTick period converted to clock counts. */
#define SYST_TICK_PERIOD_COUNT (SYST_TICK_PERIOD * MSPM0_CLOCK_FREQUENCY)

#endif // KIRB_H_
