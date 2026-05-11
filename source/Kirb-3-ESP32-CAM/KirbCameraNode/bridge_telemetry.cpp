#include "bridge_telemetry.h"

#include <Arduino.h>

#include <stdio.h>
#include <string.h>

/** @file bridge_telemetry.cpp
 *  @brief Parses MSPM0 CSV telemetry and exposes dashboard JSON snapshots.
 */

namespace {

constexpr uint32_t kTelemetryBaud = 115200;
constexpr int kTelemetryRxPin = 15;
constexpr int kTelemetryTxPin = 14;
constexpr size_t kTelemetryLineBufferLen = 448;
constexpr uint32_t kTelemetryFreshMs = 1000;
constexpr size_t kConsoleLineCount = 16;
constexpr size_t kConsoleLineLen = 96;

HardwareSerial &kTelemetrySerial = Serial2;
portMUX_TYPE gTelemetryMux = portMUX_INITIALIZER_UNLOCKED;

/** Cached copy of the latest parsed MSPM0 telemetry frame. */
struct TelemetrySnapshot {
  uint16_t rc[6];
  uint16_t ult[10];
  uint16_t ultEn[10];
  uint16_t mtr[2];
  uint16_t srv[2];
  uint16_t batMv;
  int16_t imuAccelMg[3];
  int16_t imuGyroMdps[3];
  int32_t enc[2];
  bool imuOk;
  bool encValid[2];
  uint32_t lastUpdateMs;
  bool hasData;
};

TelemetrySnapshot gTelemetry = {};
char gLineBuffer[kTelemetryLineBufferLen];
size_t gLineLength = 0;
char gConsoleLines[kConsoleLineCount][kConsoleLineLen];
uint32_t gConsoleSeq[kConsoleLineCount];
uint32_t gNextConsoleSeq = 1;

/** Add one bridge response line to the bounded console ring. */
void pushConsoleLine(const char *line) {
  if (line == nullptr || *line == '\0') {
    return;
  }

  portENTER_CRITICAL(&gTelemetryMux);
  const size_t slot = (gNextConsoleSeq - 1) % kConsoleLineCount;
  strncpy(gConsoleLines[slot], line, kConsoleLineLen - 1);
  gConsoleLines[slot][kConsoleLineLen - 1] = '\0';
  gConsoleSeq[slot] = gNextConsoleSeq;
  gNextConsoleSeq++;
  portEXIT_CRITICAL(&gTelemetryMux);
}

/** Append a JSON-escaped string into an existing buffer. */
void jsonEscapeAppend(char *dest, size_t destLen, size_t &used, const char *src) {
  while (*src != '\0' && used + 1 < destLen) {
    const char ch = *src++;
    const char *escape = nullptr;

    switch (ch) {
      case '\\':
        escape = "\\\\";
        break;
      case '"':
        escape = "\\\"";
        break;
      case '\n':
        escape = "\\n";
        break;
      case '\r':
        escape = "\\r";
        break;
      case '\t':
        escape = "\\t";
        break;
      default:
        break;
    }

    if (escape != nullptr) {
      while (*escape != '\0' && used + 1 < destLen) {
        dest[used++] = *escape++;
      }
    } else {
      dest[used++] = ch;
    }
  }
}

/** Parse one unsigned 16-bit decimal CSV token. */
bool parseUint16Token(char *token, uint16_t &value) {
  if (token == nullptr || *token == '\0') {
    return false;
  }

  char *end = nullptr;
  unsigned long parsed = strtoul(token, &end, 10);
  if (end == token || *end != '\0' || parsed > 0xFFFFu) {
    return false;
  }

  value = static_cast<uint16_t>(parsed);
  return true;
}

/** Parse one signed 16-bit decimal CSV token. */
bool parseInt16Token(char *token, int16_t &value) {
  if (token == nullptr || *token == '\0') {
    return false;
  }

  char *end = nullptr;
  long parsed = strtol(token, &end, 10);
  if (end == token || *end != '\0' || parsed < -32768L || parsed > 32767L) {
    return false;
  }

  value = static_cast<int16_t>(parsed);
  return true;
}

/** Parse one signed 32-bit decimal CSV token. */
bool parseInt32Token(char *token, int32_t &value) {
  if (token == nullptr || *token == '\0') {
    return false;
  }

  char *end = nullptr;
  long parsed = strtol(token, &end, 10);
  if (end == token || *end != '\0') {
    return false;
  }

  value = static_cast<int32_t>(parsed);
  return true;
}

/** Parse a fixed-length unsigned telemetry section. */
bool parseFixedSection(char **context, uint16_t *dest, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    char *token = strtok_r(nullptr, ",", context);
    if (!parseUint16Token(token, dest[i])) {
      return false;
    }
  }

  return true;
}

/** Parse a fixed-length signed 16-bit telemetry section. */
bool parseFixedSectionInt16(char **context, int16_t *dest, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    char *token = strtok_r(nullptr, ",", context);
    if (!parseInt16Token(token, dest[i])) {
      return false;
    }
  }

  return true;
}

/** Parse one TEL CSV frame into a telemetry snapshot. */
bool parseTelemetryLine(char *line, TelemetrySnapshot &out) {
  char *context = nullptr;
  char *token = strtok_r(line, ",", &context);
  if (token == nullptr || strcmp(token, "TEL") != 0) {
    return false;
  }

  bool sawRc = false;
  bool sawUlt = false;
  bool sawUltEn = false;
  bool sawMtr = false;
  bool sawSrv = false;
  bool sawBat = false;
  bool sawImuOk = false;
  bool sawImu = false;
  bool sawEncOk = false;
  bool sawEnc = false;

  while ((token = strtok_r(nullptr, ",", &context)) != nullptr) {
    if (strcmp(token, "RC") == 0) {
      sawRc = parseFixedSection(&context, out.rc, 6);
      if (!sawRc) {
        return false;
      }
    } else if (strcmp(token, "ULT") == 0) {
      sawUlt = parseFixedSection(&context, out.ult, 10);
      if (!sawUlt) {
        return false;
      }
    } else if (strcmp(token, "ULT_EN") == 0) {
      sawUltEn = parseFixedSection(&context, out.ultEn, 10);
      if (!sawUltEn) {
        return false;
      }
    } else if (strcmp(token, "MTR") == 0) {
      sawMtr = parseFixedSection(&context, out.mtr, 2);
      if (!sawMtr) {
        return false;
      }
    } else if (strcmp(token, "SRV") == 0) {
      sawSrv = parseFixedSection(&context, out.srv, 2);
      if (!sawSrv) {
        return false;
      }
    } else if (strcmp(token, "BAT") == 0) {
      char *batToken = strtok_r(nullptr, ",", &context);
      sawBat = parseUint16Token(batToken, out.batMv);
      if (!sawBat) {
        return false;
      }
    } else if (strcmp(token, "IMU_OK") == 0) {
      uint16_t imuOk = 0;
      char *imuOkToken = strtok_r(nullptr, ",", &context);
      sawImuOk = parseUint16Token(imuOkToken, imuOk);
      if (!sawImuOk) {
        return false;
      }
      out.imuOk = (imuOk != 0);
    } else if (strcmp(token, "IMU") == 0) {
      sawImu = parseFixedSectionInt16(&context, out.imuAccelMg, 3);
      if (!sawImu) {
        return false;
      }
      sawImu = parseFixedSectionInt16(&context, out.imuGyroMdps, 3);
      if (!sawImu) {
        return false;
      }
    } else if (strcmp(token, "ENC_OK") == 0) {
      uint16_t encOk0 = 0;
      uint16_t encOk1 = 0;
      char *nextToken = nullptr;
      sawEncOk = parseUint16Token(strtok_r(nullptr, ",", &context), encOk0);
      if (!sawEncOk) {
        return false;
      }
      nextToken = strtok_r(nullptr, ",", &context);
      if (parseUint16Token(nextToken, encOk1)) {
        out.encValid[0] = (encOk0 != 0);
        out.encValid[1] = (encOk1 != 0);
      } else if (nextToken != nullptr && strcmp(nextToken, "ENC") == 0) {
        out.encValid[0] = (encOk0 != 0);
        out.encValid[1] = (encOk0 != 0);
        sawEnc = parseInt32Token(strtok_r(nullptr, ",", &context), out.enc[0]);
        if (!sawEnc) {
          return false;
        }
        sawEnc = parseInt32Token(strtok_r(nullptr, ",", &context), out.enc[1]);
        if (!sawEnc) {
          return false;
        }
      } else {
        return false;
      }
    } else if (strcmp(token, "ENC") == 0) {
      sawEnc = parseInt32Token(strtok_r(nullptr, ",", &context), out.enc[0]);
      if (!sawEnc) {
        return false;
      }
      sawEnc = parseInt32Token(strtok_r(nullptr, ",", &context), out.enc[1]);
      if (!sawEnc) {
        return false;
      }
    } else {
      return false;
    }
  }

  if (!(sawRc && sawUlt && sawUltEn && sawMtr && sawSrv && sawBat && sawImuOk && sawImu &&
        sawEncOk && sawEnc)) {
    return false;
  }

  out.lastUpdateMs = millis();
  out.hasData = true;
  return true;
}

/** Publish a parsed snapshot under the telemetry mutex. */
void commitTelemetry(const TelemetrySnapshot &snapshot) {
  portENTER_CRITICAL(&gTelemetryMux);
  gTelemetry = snapshot;
  portEXIT_CRITICAL(&gTelemetryMux);
}

/** Copy the latest snapshot under the telemetry mutex. */
TelemetrySnapshot copyTelemetry() {
  TelemetrySnapshot snapshot;
  portENTER_CRITICAL(&gTelemetryMux);
  snapshot = gTelemetry;
  portEXIT_CRITICAL(&gTelemetryMux);
  return snapshot;
}

/** Classify and consume a complete UART line from the MSPM0. */
void handleCompleteLine() {
  gLineBuffer[gLineLength] = '\0';

  if (strncmp(gLineBuffer, "ACK,", 4) == 0 || strncmp(gLineBuffer, "ERR,", 4) == 0 || strncmp(gLineBuffer, "LOG,", 4) == 0
      || strncmp(gLineBuffer, "RSP,", 4) == 0
      || strncmp(gLineBuffer, "STS,", 4) == 0) {
    pushConsoleLine(gLineBuffer);
  } else {
    TelemetrySnapshot parsed = copyTelemetry();
    if (parseTelemetryLine(gLineBuffer, parsed)) {
    commitTelemetry(parsed);
    } else {
      Serial.print("[bridge] dropped telemetry line: ");
      Serial.println(gLineBuffer);
    }
  }

  gLineLength = 0;
}

}  // namespace

/** Start the UART used for MSPM0 telemetry and commands. */
void bridgeTelemetryBegin() {
  kTelemetrySerial.begin(kTelemetryBaud, SERIAL_8N1, kTelemetryRxPin, kTelemetryTxPin);
  gLineLength = 0;
}

/** Poll the UART and process complete telemetry/console lines. */
void bridgeTelemetryPoll() {
  while (kTelemetrySerial.available() > 0) {
    char ch = static_cast<char>(kTelemetrySerial.read());

    if (ch == '\r') {
      continue;
    }

    if (ch == '\n') {
      if (gLineLength > 0) {
        handleCompleteLine();
      }
      continue;
    }

    if (gLineLength + 1 >= kTelemetryLineBufferLen) {
      gLineLength = 0;
      continue;
    }

    gLineBuffer[gLineLength++] = ch;
  }
}

/** Build a JSON document for the latest telemetry snapshot. */
size_t bridgeTelemetryBuildJson(char *buffer, size_t bufferLen) {
  const TelemetrySnapshot snapshot = copyTelemetry();
  const bool fresh = snapshot.hasData && (millis() - snapshot.lastUpdateMs) <= kTelemetryFreshMs;

  return static_cast<size_t>(snprintf(
    buffer, bufferLen,
    "{\"connected\":%s,\"last_update_ms\":%lu,\"rc\":[%u,%u,%u,%u,%u,%u],"
    "\"ult\":[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u],"
    "\"ult_en\":[%u,%u,%u,%u,%u,%u,%u,%u,%u,%u],"
    "\"mtr\":[%u,%u],\"srv\":[%u,%u],\"bat_mv\":%u,"
    "\"imu_ok\":%s,\"imu_accel_mg\":[%d,%d,%d],\"imu_gyro_mdps\":[%d,%d,%d],"
    "\"enc_valid\":[%s,%s],\"enc\":[%ld,%ld]}",
    fresh ? "true" : "false",
    static_cast<unsigned long>(snapshot.lastUpdateMs),
    snapshot.rc[0], snapshot.rc[1], snapshot.rc[2], snapshot.rc[3], snapshot.rc[4], snapshot.rc[5],
    snapshot.ult[0], snapshot.ult[1], snapshot.ult[2], snapshot.ult[3], snapshot.ult[4],
    snapshot.ult[5], snapshot.ult[6], snapshot.ult[7], snapshot.ult[8], snapshot.ult[9],
    snapshot.ultEn[0], snapshot.ultEn[1], snapshot.ultEn[2], snapshot.ultEn[3], snapshot.ultEn[4],
    snapshot.ultEn[5], snapshot.ultEn[6], snapshot.ultEn[7], snapshot.ultEn[8], snapshot.ultEn[9],
    snapshot.mtr[0], snapshot.mtr[1], snapshot.srv[0], snapshot.srv[1], snapshot.batMv,
    snapshot.imuOk ? "true" : "false",
    static_cast<int>(snapshot.imuAccelMg[0]), static_cast<int>(snapshot.imuAccelMg[1]), static_cast<int>(snapshot.imuAccelMg[2]),
    static_cast<int>(snapshot.imuGyroMdps[0]), static_cast<int>(snapshot.imuGyroMdps[1]), static_cast<int>(snapshot.imuGyroMdps[2]),
    snapshot.encValid[0] ? "true" : "false", snapshot.encValid[1] ? "true" : "false",
    static_cast<long>(snapshot.enc[0]), static_cast<long>(snapshot.enc[1])));
}

/** Return true when a telemetry frame arrived recently. */
bool bridgeTelemetryHasFreshData() {
  const TelemetrySnapshot snapshot = copyTelemetry();
  return snapshot.hasData && (millis() - snapshot.lastUpdateMs) <= kTelemetryFreshMs;
}

/** Queue a command line to the MSPM0 and return an immediate status string. */
bool bridgeTelemetryQueueCommand(const char *command, char *response, size_t responseLen) {
  if (command == nullptr || *command == '\0') {
    if (response != nullptr && responseLen > 0) {
      snprintf(response, responseLen, "empty command");
    }
    return false;
  }

  Serial.print("[bridge] command stub received: ");
  Serial.println(command);

  kTelemetrySerial.print("CMD,");
  kTelemetrySerial.print(command);
  kTelemetrySerial.print('\n');

  if (response != nullptr && responseLen > 0) {
    snprintf(response, responseLen, "queued");
  }

  return true;
}

/** Build JSON console output containing lines newer than sinceSeq. */
size_t bridgeTelemetryBuildConsoleJson(uint32_t sinceSeq, char *buffer, size_t bufferLen) {
  uint32_t nextSeq = 0;
  uint32_t seqCopy[kConsoleLineCount];
  char lineCopy[kConsoleLineCount][kConsoleLineLen];

  portENTER_CRITICAL(&gTelemetryMux);
  nextSeq = gNextConsoleSeq;
  for (size_t i = 0; i < kConsoleLineCount; ++i) {
    seqCopy[i] = gConsoleSeq[i];
    strncpy(lineCopy[i], gConsoleLines[i], kConsoleLineLen);
    lineCopy[i][kConsoleLineLen - 1] = '\0';
  }
  portEXIT_CRITICAL(&gTelemetryMux);

  size_t used = 0;
  int written = snprintf(buffer, bufferLen, "{\"next\":%lu,\"lines\":[",
                         static_cast<unsigned long>(nextSeq));
  if (written < 0) {
    return 0;
  }
  used = static_cast<size_t>(written);

  bool first = true;
  for (uint32_t seq = sinceSeq + 1; seq < nextSeq; ++seq) {
    const size_t slot = (seq - 1) % kConsoleLineCount;
    if (seqCopy[slot] != seq) {
      continue;
    }

    if (!first && used + 1 < bufferLen) {
      buffer[used++] = ',';
    }
    first = false;

    if (used + 1 < bufferLen) {
      buffer[used++] = '"';
    }
    jsonEscapeAppend(buffer, bufferLen, used, lineCopy[slot]);
    if (used + 1 < bufferLen) {
      buffer[used++] = '"';
    }
  }

  if (used + 2 < bufferLen) {
    buffer[used++] = ']';
    buffer[used++] = '}';
    buffer[used] = '\0';
  }

  return used;
}
