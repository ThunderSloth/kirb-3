#ifndef TELEMETRY_H
#define TELEMETRY_H

/** @file telemetry.h
 *  @brief Periodic MSPM0-to-ESP32 telemetry frame sender.
 */

#include <stdint.h>

/** Clear telemetry scheduler state. */
void telemetry_init(void);

/** Mark one telemetry frame as due from SysTick. */
void telemetry_schedule(void);

/** Send a telemetry frame when scheduled. */
void telemetry_service(void);

#endif // TELEMETRY_H
