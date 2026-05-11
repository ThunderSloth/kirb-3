#ifndef BRIDGE_TELEMETRY_H
#define BRIDGE_TELEMETRY_H

/** @file bridge_telemetry.h
 *  @brief ESP32-CAM-side parser and JSON builder for MSPM0 telemetry.
 */

#include <stddef.h>
#include <stdint.h>

/** Start the UART used for MSPM0 telemetry and commands. */
void bridgeTelemetryBegin();

/** Poll the UART and process complete telemetry/console lines. */
void bridgeTelemetryPoll();

/** Build a JSON document for the latest telemetry snapshot. */
size_t bridgeTelemetryBuildJson(char *buffer, size_t bufferLen);

/** Return true when a telemetry frame arrived recently. */
bool bridgeTelemetryHasFreshData();

/** Queue a command line to the MSPM0 and return an immediate status string. */
bool bridgeTelemetryQueueCommand(const char *command, char *response, size_t responseLen);

/** Build JSON console output containing lines newer than sinceSeq. */
size_t bridgeTelemetryBuildConsoleJson(uint32_t sinceSeq, char *buffer, size_t bufferLen);

#endif // BRIDGE_TELEMETRY_H
