#ifndef DASHBOARD_INDEX_H
#define DASHBOARD_INDEX_H

/** @file dashboard_index.h
 *  @brief Embedded camera/bridge dashboard HTML, CSS, and JavaScript.
 */

static const char dashboard_index_html[] = R"rawliteral(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>KirbCamBridge</title>
  <style>
    :root {
      --bg: #0f1418;
      --panel: rgba(23, 29, 35, 0.9);
      --panel-strong: rgba(17, 22, 27, 0.98);
      --line: rgba(217, 143, 83, 0.16);
      --text: #d8dee7;
      --muted: #93a0ad;
      --accent: #e38b45;
      --accent-strong: #b85a2b;
      --ok: #9fe870;
      --shadow: 0 22px 48px rgba(0, 0, 0, 0.34);
      --radius: 20px;
      --center-col-max: 610px;
    }

    * {
      box-sizing: border-box;
    }

    body {
      margin: 0;
      min-height: 100vh;
      font-family: "Trebuchet MS", "Segoe UI", sans-serif;
      color: var(--text);
      background:
        radial-gradient(circle at top left, rgba(227, 139, 69, 0.1), transparent 24%),
        radial-gradient(circle at top right, rgba(159, 232, 112, 0.08), transparent 28%),
        linear-gradient(180deg, #0f1418 0%, #131a20 48%, #10161b 100%);
    }

    .shell {
      width: min(1300px, calc(100vw - 20px));
      margin: 0 auto;
      padding: 12px 0 18px;
    }

    .hero {
      display: grid;
      grid-template-columns: auto 1fr auto;
      gap: 14px;
      align-items: center;
      margin-bottom: 12px;
      padding: 12px 16px;
      border: 1px solid rgba(227, 139, 69, 0.12);
      border-radius: 26px;
      background:
        linear-gradient(135deg, rgba(28, 36, 43, 0.96), rgba(19, 25, 31, 0.92));
      box-shadow: var(--shadow);
    }

    .hero-center {
      text-align: center;
    }

    .title-main {
      margin: 0;
      color: #d8dee7;
      font-family: "Helvetica Neue", "Helvetica", "Arial", sans-serif;
      font-size: clamp(1.8rem, 4.1vw, 3.1rem);
      line-height: 1;
      font-weight: 600;
      letter-spacing: 0.08em;
      text-transform: uppercase;
    }

    .status-pill {
      --pill-color: var(--ok);
      --pill-ring: rgba(159, 232, 112, 0.12);
      --pill-pulse: pulse 1.8s infinite;
      display: inline-flex;
      align-items: center;
      gap: 10px;
      padding: 8px 12px;
      border-radius: 999px;
      background: rgba(20, 26, 31, 0.92);
      border: 1px solid rgba(227, 139, 69, 0.14);
      color: var(--ok);
      font-weight: 700;
      white-space: nowrap;
      box-shadow: inset 0 0 0 1px rgba(227, 139, 69, 0.03);
    }

    .status-pill::before {
      content: "";
      width: 11px;
      height: 11px;
      border-radius: 50%;
      background: var(--pill-color);
      box-shadow: 0 0 0 4px var(--pill-ring);
      animation: var(--pill-pulse);
    }

    .layout {
      display: grid;
      grid-template-columns: minmax(210px, 260px) minmax(0, 1fr) minmax(210px, 260px);
      gap: 12px;
      align-items: start;
    }

    .dashboard-scroll {
      overflow: visible;
      padding-right: 0;
    }

    .log::-webkit-scrollbar {
      width: 10px;
      height: 10px;
    }

    .log::-webkit-scrollbar-track {
      background: rgba(12, 17, 21, 0.8);
      border-radius: 999px;
    }

    .log::-webkit-scrollbar-thumb {
      background: rgba(227, 139, 69, 0.45);
      border-radius: 999px;
      border: 2px solid rgba(12, 17, 21, 0.8);
    }

    .layout > .stack:nth-child(2) {
      width: 100%;
      max-width: var(--center-col-max);
      justify-self: center;
    }

    .panel {
      border: 1px solid rgba(227, 139, 69, 0.08);
      border-radius: var(--radius);
      background: var(--panel);
      box-shadow: var(--shadow);
      overflow: hidden;
      backdrop-filter: blur(6px);
    }

    .panel-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      padding: 9px 14px 7px;
      border-bottom: 1px solid rgba(227, 139, 69, 0.07);
      background: linear-gradient(180deg, rgba(227, 139, 69, 0.03), rgba(227, 139, 69, 0));
    }

    .panel-header h2,
    .panel-header h3 {
      margin: 0;
      font-size: 0.95rem;
      text-transform: uppercase;
      letter-spacing: 0.08em;
      color: #dde7f3;
    }

    .panel-tools {
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .unit-select {
      border: 1px solid rgba(227, 139, 69, 0.12);
      border-radius: 999px;
      background: rgba(20, 26, 31, 0.92);
      color: #d8dee7;
      font: inherit;
      padding: 6px 12px;
      min-height: 34px;
      cursor: pointer;
    }

    .unit-select:focus {
      outline: none;
      border-color: rgba(227, 139, 69, 0.28);
      box-shadow: 0 0 0 3px rgba(227, 139, 69, 0.08);
    }

    .panel-body {
      padding: 9px 14px 10px;
    }

    .stream-panel {
      background: linear-gradient(180deg, rgba(11, 16, 20, 0.98), rgba(14, 20, 24, 0.96));
      color: #d8dee7;
    }

    .stream-wrap {
      padding: 12px;
    }

    .stream-stage {
      position: relative;
      aspect-ratio: 1.18 / 1;
      border-radius: 20px;
      overflow: hidden;
      background:
        radial-gradient(circle at top, rgba(227, 139, 69, 0.08), transparent 34%),
        #080b0e;
      border: 1px solid rgba(227, 139, 69, 0.12);
      box-shadow: inset 0 0 0 1px rgba(227, 139, 69, 0.04);
    }

    .stream-stage img {
      width: 100%;
      height: 100%;
      display: block;
      object-fit: fill;
      background: #06090c;
    }

    .metric-grid {
      display: grid;
      grid-template-columns: 1fr;
      gap: 8px;
    }

    .stack {
      display: grid;
      gap: 12px;
    }

    .metric-row {
      display: grid;
      grid-template-columns: 56px 1fr 58px;
      gap: 12px;
      align-items: center;
      padding: 6px 10px;
      border-radius: 14px;
      background: rgba(27, 34, 40, 0.9);
      border: 1px solid rgba(227, 139, 69, 0.06);
    }

    .metric-name {
      font-weight: 700;
      color: #c8d2de;
    }

    .metric-bar {
      height: 8px;
      border-radius: 999px;
      background: rgba(227, 139, 69, 0.12);
      overflow: hidden;
    }

    .metric-fill {
      height: 100%;
      width: 50%;
      border-radius: inherit;
      background: linear-gradient(90deg, #7c5a35, var(--accent));
      box-shadow: 0 0 12px rgba(227, 139, 69, 0.12);
      transition: width 220ms ease;
    }

    .metric-row.metric-disabled {
      background: rgba(22, 27, 32, 0.7);
      border-color: rgba(147, 160, 173, 0.12);
    }

    .metric-row.metric-disabled .metric-name,
    .metric-row.metric-disabled .metric-value {
      color: var(--muted);
    }

    .metric-row.metric-disabled .metric-bar {
      background: rgba(147, 160, 173, 0.12);
    }

    .metric-row.metric-disabled .metric-fill {
      width: 0%;
      background: linear-gradient(90deg, rgba(147, 160, 173, 0.35), rgba(147, 160, 173, 0.55));
      box-shadow: none;
    }

    .metric-bar-centered {
      position: relative;
      height: 8px;
      border-radius: 999px;
      background: rgba(227, 139, 69, 0.1);
      overflow: hidden;
    }

    .metric-bar-centered::before {
      content: "";
      position: absolute;
      left: calc(50% - 1px);
      top: 0;
      width: 2px;
      height: 100%;
      background: rgba(216, 222, 231, 0.35);
    }

    .metric-fill-centered {
      position: absolute;
      top: 0;
      height: 100%;
      background: linear-gradient(90deg, #7c5a35, var(--accent));
      box-shadow: 0 0 12px rgba(227, 139, 69, 0.12);
      transition: left 220ms ease, width 220ms ease;
    }

    .metric-value {
      text-align: right;
      font-variant-numeric: tabular-nums;
      font-weight: 700;
    }

    .pairs {
      display: grid;
      grid-template-columns: repeat(2, minmax(0, 1fr));
      gap: 10px;
    }

    .pair {
      padding: 12px;
      border-radius: 16px;
      background: rgba(255, 255, 255, 0.68);
      border: 1px solid rgba(166, 61, 28, 0.08);
    }

    .pair label {
      display: block;
      font-size: 0.8rem;
      color: var(--muted);
      text-transform: uppercase;
      letter-spacing: 0.06em;
      margin-bottom: 8px;
    }

    .pair strong {
      display: block;
      font-size: 1.35rem;
      font-variant-numeric: tabular-nums;
    }

    .console {
      margin-top: 0;
    }

    .console .panel-body {
      display: grid;
      gap: 10px;
    }

    .command-row {
      display: grid;
      grid-template-columns: auto minmax(0, 1fr) auto;
      gap: 10px;
      align-items: center;
    }

    .prompt-prefix {
      display: inline-flex;
      align-items: center;
      justify-content: center;
      min-width: 44px;
      height: 48px;
      border-radius: 14px;
      border: 1px solid rgba(159, 232, 112, 0.18);
      background: rgba(159, 232, 112, 0.08);
      color: #bdf58a;
      font: 1.05rem/1 "Courier New", monospace;
      font-weight: 700;
    }

    .command-row input {
      width: 100%;
      border: 1px solid rgba(159, 232, 112, 0.18);
      border-radius: 14px;
      background: var(--panel-strong);
      color: #d7ffb9;
      padding: 14px 16px;
      font: inherit;
      box-shadow: inset 0 0 0 1px rgba(159, 232, 112, 0.05);
      caret-color: #9fe870;
    }

    .command-row input:focus {
      outline: none;
      border-color: rgba(159, 232, 112, 0.34);
      box-shadow:
        inset 0 0 0 1px rgba(159, 232, 112, 0.08),
        0 0 0 3px rgba(159, 232, 112, 0.08);
    }

    .command-row button {
      border: 0;
      border-radius: 14px;
      background: linear-gradient(135deg, #4f9a35, #9fe870);
      color: #081018;
      font: inherit;
      font-weight: 700;
      min-height: 48px;
      padding: 0 18px;
      cursor: pointer;
    }

    .command-row button:hover {
      filter: brightness(1.04);
    }

    .log {
      height: clamp(118px, 18vh, 210px);
      min-height: 0;
      margin: 0;
      padding: 12px;
      border-radius: 16px;
      background:
        linear-gradient(180deg, rgba(12, 17, 21, 0.98), rgba(14, 20, 24, 0.98));
      color: #d7ffb9;
      overflow: auto;
      font: 0.92rem/1.45 "Courier New", monospace;
      white-space: pre-wrap;
      border: 1px solid rgba(227, 139, 69, 0.08);
      scrollbar-color: rgba(159, 232, 112, 0.42) rgba(12, 17, 21, 0.8);
      scrollbar-width: thin;
    }

    .stream-controls {
      display: flex;
      gap: 10px;
      align-items: center;
    }

    .stream-button {
      border: 1px solid rgba(227, 139, 69, 0.12);
      border-radius: 999px;
      background: rgba(227, 139, 69, 0.08);
      color: #eaf4ff;
      font: inherit;
      font-weight: 700;
      padding: 8px 14px;
      cursor: pointer;
    }

    .stream-button:hover {
      background: rgba(227, 139, 69, 0.14);
    }

    @keyframes pulse {
      0% { transform: scale(0.95); opacity: 0.85; }
      70% { transform: scale(1.05); opacity: 1; }
      100% { transform: scale(0.95); opacity: 0.85; }
    }

    @media (max-width: 1080px) {
      .layout {
        grid-template-columns: 1fr;
      }

      .hero {
        grid-template-columns: 1fr;
      }

      .hero-center {
        text-align: left;
      }

      .title-main {
        font-size: clamp(1.65rem, 3.9vw, 2.6rem);
        letter-spacing: 0.06em;
      }
    }

    @media (max-width: 640px) {
      .shell {
        width: min(100vw - 16px, 100%);
        padding-top: 10px;
      }

      .hero,
      .panel-header,
      .panel-body,
      .stream-wrap {
        padding-left: 14px;
        padding-right: 14px;
      }

      .pairs {
        grid-template-columns: 1fr;
      }

      .command-row {
        grid-template-columns: 1fr;
      }

      .prompt-prefix {
        display: none;
      }
    }

    @media (orientation: landscape) and (max-height: 520px) {
      body {
        overflow: hidden;
      }

      .shell {
        width: 100vw;
        height: 100vh;
        padding: 5px;
        display: grid;
        grid-template-rows: auto minmax(0, 1fr);
        gap: 5px;
      }

      .hero {
        margin: 0;
        padding: 4px 6px;
        grid-template-columns: minmax(112px, 0.75fr) minmax(140px, 1fr) minmax(112px, 0.75fr);
        gap: 5px;
        border-radius: 14px;
      }

      .hero-center {
        text-align: center;
        min-width: 0;
      }

      .title-main {
        font-size: clamp(0.76rem, 2.7vw, 1rem);
        letter-spacing: 0.035em;
        white-space: nowrap;
      }

      .status-pill {
        min-width: 0;
        justify-content: center;
        padding: 4px 5px;
        gap: 4px;
        font-size: 0.62rem;
        overflow: hidden;
        text-overflow: ellipsis;
      }

      .status-pill::before {
        width: 7px;
        height: 7px;
        box-shadow: 0 0 0 3px var(--pill-ring);
      }

      .layout {
        min-height: 0;
        grid-template-columns: minmax(138px, 0.72fr) minmax(300px, 1.7fr) minmax(138px, 0.72fr);
        gap: 5px;
      }

      .stack {
        min-height: 0;
        gap: 5px;
      }

      .panel {
        min-height: 0;
        border-radius: 12px;
      }

      .panel-header {
        padding: 5px 8px;
      }

      .panel-header h2,
      .panel-header h3 {
        font-size: 0.66rem;
        letter-spacing: 0.06em;
      }

      .panel-header span,
      .stream-controls {
        font-size: 0.64rem;
      }

      .panel-body {
        padding: 6px 8px;
      }

      .stream-wrap {
        padding: 5px;
      }

      .stream-stage {
        height: calc(100vh - 98px);
        min-height: 0;
        aspect-ratio: auto;
        border-radius: 12px;
      }

      .stream-stage img {
        image-rendering: pixelated;
        image-rendering: crisp-edges;
        object-fit: fill;
      }

      .metric-grid {
        gap: 4px;
      }

      .metric-row {
        grid-template-columns: 38px 1fr 42px;
        gap: 5px;
        padding: 4px 5px;
        border-radius: 9px;
      }

      .metric-name,
      .metric-value {
        font-size: 0.66rem;
      }

      .metric-bar,
      .metric-bar-centered {
        height: 6px;
      }

      .console {
        margin-top: 0;
        min-height: 0;
        align-self: stretch;
      }

      .console .panel-header {
        display: none;
      }

      .console .panel-body {
        display: grid;
        grid-template-columns: minmax(0, 1fr) minmax(185px, 250px);
        align-items: center;
        gap: 6px;
        padding: 5px;
      }

      .log {
        height: 42px;
        min-height: 0;
        max-height: none;
        padding: 5px 7px;
        border-radius: 9px;
        font-size: 0.62rem;
        line-height: 1.1;
      }

      .command-row {
        grid-template-columns: minmax(0, 1fr) auto;
        gap: 5px;
      }

      .prompt-prefix {
        display: none;
      }

      .command-row input {
        min-height: 30px;
        padding: 6px 8px;
        border-radius: 9px;
        font-size: 0.72rem;
      }

      .command-row button,
      .stream-button {
        min-height: 30px;
        padding: 0 10px;
        border-radius: 999px;
        font-size: 0.66rem;
      }
    }
  </style>
</head>
<body>
  <div class="shell">
    <section class="hero">
      <div class="status-pill" id="telemetry-stamp">Last Update --:--:--</div>
      <div class="hero-center">
        <h1 class="title-main">KIRB-3 DASHBOARD</h1>
      </div>
      <div class="status-pill" id="bridge-status">UI Online</div>
    </section>

    <section class="dashboard-scroll" aria-label="Telemetry and video panels">
      <main class="layout">
        <div class="stack" id="left-stack">
          <section class="panel">
            <div class="panel-header">
              <h2>RC Inputs</h2>
              <span>µs</span>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="rc-grid"></div>
            </div>
          </section>

          <section class="panel">
            <div class="panel-header">
              <h2>Drive Outputs</h2>
              <span>µs</span>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="drive-grid"></div>
            </div>
          </section>

          <section class="panel">
            <div class="panel-header">
              <h2>Accel</h2>
              <span>g</span>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="accel-grid"></div>
            </div>
          </section>

          <section class="panel">
            <div class="panel-header">
              <h2>Gyro</h2>
              <span>dps</span>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="gyro-grid"></div>
            </div>
          </section>
        </div>

        <div class="stack" id="center-stack">
          <section class="panel stream-panel">
            <div class="panel-header">
              <h2>Video Feed</h2>
              <div class="stream-controls">
                <span id="stream-host">Awaiting stream</span>
                <button class="stream-button" id="stream-toggle" type="button">Stop Stream</button>
              </div>
            </div>
            <div class="stream-wrap">
              <div class="stream-stage" id="stream-stage">
                <img id="stream" alt="Kirb live stream">
              </div>
            </div>
          </section>

          <section class="panel console">
            <div class="panel-header">
              <h3>Command Console</h3>
              <span></span>
            </div>
            <div class="panel-body">
              <pre class="log" id="command-log">[boot] KirbCamBridge dashboard loaded
[link] Waiting for MSP telemetry
</pre>

              <form id="command-form">
                <div class="command-row">
                  <span class="prompt-prefix">&gt;</span>
                  <input id="command-input" name="command" type="text" placeholder="">
                  <button type="submit">Send</button>
                </div>
              </form>
            </div>
          </section>
        </div>

        <div class="stack">
          <section class="panel">
            <div class="panel-header">
              <h2>Power</h2>
              <span>V</span>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="power-grid"></div>
            </div>
          </section>

          <section class="panel">
            <div class="panel-header">
              <h2>Ultrasonic</h2>
              <div class="panel-tools">
                <select class="unit-select" id="ult-unit-select">
                  <option value="cm" selected>cm</option>
                  <option value="m">m</option>
                  <option value="ftin">ft/in</option>
                  <option value="pw">pw (us)</option>
                </select>
              </div>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="ult-grid"></div>
            </div>
          </section>

          <section class="panel">
            <div class="panel-header">
              <h2>Encoders</h2>
              <span>counts</span>
            </div>
            <div class="panel-body">
              <div class="metric-grid" id="encoder-grid"></div>
            </div>
          </section>
        </div>
      </main>
    </section>
  </div>

  <script>
    const rcGrid = document.getElementById("rc-grid");
    const driveGrid = document.getElementById("drive-grid");
    const ultGrid = document.getElementById("ult-grid");
    const powerGrid = document.getElementById("power-grid");
    const accelGrid = document.getElementById("accel-grid");
    const gyroGrid = document.getElementById("gyro-grid");
    const encoderGrid = document.getElementById("encoder-grid");
    const streamImg = document.getElementById("stream");
    const streamStage = document.getElementById("stream-stage");
    const streamHost = document.getElementById("stream-host");
    const bridgeStatus = document.getElementById("bridge-status");
    const telemetryStamp = document.getElementById("telemetry-stamp");
    const ultUnitSelect = document.getElementById("ult-unit-select");
    const streamToggle = document.getElementById("stream-toggle");
    const commandForm = document.getElementById("command-form");
    const commandInput = document.getElementById("command-input");
    const commandLog = document.getElementById("command-log");
    const leftStack = document.getElementById("left-stack");
    const centerStack = document.getElementById("center-stack");
    let streamUrl = "/capture";
    let streamRevision = 0;
    let streamEnabled = false;
    let streamTimer = null;
    let streamTimeout = null;
    let streamRequestInFlight = false;
    let streamFrame = null;
    let telemetryTimer = null;
    let telemetryOnline = false;
    let consoleSeq = 0;
    let telemetryInFlight = false;
    let consoleInFlight = false;
    let latestTelemetry = null;
    let ultDisplayUnit = "cm";
    const telemetryPollMs = 100;
    const consolePollMs = 2000;
    const snapshotPollMs = 333;
    const snapshotTimeoutMs = 2500;

    function buildMetricRows(container, prefix, count) {
      for (let i = 0; i < count; i += 1) {
        const row = document.createElement("div");
        row.className = "metric-row";
        row.id = prefix.toLowerCase() + "-row-" + i;
        row.innerHTML =
          '<span class="metric-name">' + prefix + i + '</span>' +
          '<div class="metric-bar"><div class="metric-fill" id="' + prefix.toLowerCase() + '-fill-' + i + '"></div></div>' +
          '<span class="metric-value" id="' + prefix.toLowerCase() + '-value-' + i + '">0</span>';
        container.appendChild(row);
      }
    }

    function buildNamedMetricRows(container, names) {
      for (let i = 0; i < names.length; i += 1) {
        const row = document.createElement("div");
        row.className = "metric-row";
        row.innerHTML =
          '<span class="metric-name">' + names[i] + '</span>' +
          '<div class="metric-bar"><div class="metric-fill" id="' + names[i].toLowerCase() + '-fill"></div></div>' +
          '<span class="metric-value" id="' + names[i].toLowerCase() + '-value">0</span>';
        container.appendChild(row);
      }
    }

    function buildCenteredMetricRows(container, names) {
      for (let i = 0; i < names.length; i += 1) {
        const row = document.createElement("div");
        row.className = "metric-row";
        row.id = names[i].toLowerCase() + "-row";
        row.innerHTML =
          '<span class="metric-name">' + names[i] + '</span>' +
          '<div class="metric-bar-centered"><div class="metric-fill-centered" id="' + names[i].toLowerCase() + '-fill"></div></div>' +
          '<span class="metric-value" id="' + names[i].toLowerCase() + '-value">0</span>';
        container.appendChild(row);
      }
    }

    function setMetricUnavailable(prefix, index) {
      const row = document.getElementById(prefix + "-row-" + index);
      const fill = document.getElementById(prefix + "-fill-" + index);
      const label = document.getElementById(prefix + "-value-" + index);

      row.classList.add("metric-disabled");
      fill.style.width = "0%";
      label.textContent = "--";
    }

    function setCenteredMetricUnavailable(name) {
      const row = document.getElementById(name.toLowerCase() + "-row");
      const fill = document.getElementById(name.toLowerCase() + "-fill");
      const label = document.getElementById(name.toLowerCase() + "-value");

      row.classList.add("metric-disabled");
      fill.style.width = "0%";
      label.textContent = "--";
    }

    buildCenteredMetricRows(rcGrid, ["RC0", "RC1", "RC2", "RC3", "RC4", "RC5"]);
    buildMetricRows(ultGrid, "ULT", 10);
    buildNamedMetricRows(powerGrid, ["BAT"]);
    buildCenteredMetricRows(accelGrid, ["AX", "AY", "AZ"]);
    buildCenteredMetricRows(gyroGrid, ["GX", "GY", "GZ"]);
    buildCenteredMetricRows(encoderGrid, ["ENC0", "ENC1"]);

    const driveNames = ["MTR0", "MTR1", "PAN", "TILT"];
    buildCenteredMetricRows(driveGrid, driveNames);

    function logLine(text) {
      const atBottom = commandLog.scrollTop + commandLog.clientHeight >= commandLog.scrollHeight - 8;
      commandLog.textContent += text + "\n";
      if (atBottom) {
        commandLog.scrollTop = commandLog.scrollHeight;
      }
    }

    function setTelemetryStamp(text) {
      telemetryStamp.textContent = text;
    }

    function tuneCenterColumnWidth() {
      if (!leftStack || !centerStack || !streamStage) {
        return;
      }

      if (window.innerWidth <= 980) {
        document.documentElement.style.setProperty("--center-col-max", "100%");
        return;
      }

      const leftHeight = leftStack.getBoundingClientRect().height;
      const centerRect = centerStack.getBoundingClientRect();
      const streamRect = streamStage.getBoundingClientRect();
      const currentHeight = centerRect.height;
      const currentWidth = centerRect.width;
      const videoHeight = streamRect.height;

      if (leftHeight <= 0 || currentHeight <= 0 || currentWidth <= 0 || videoHeight <= 0) {
        return;
      }

      const desiredVideoHeight = videoHeight + (leftHeight - currentHeight);
      const clampedVideoHeight = Math.max(220, Math.min(620, desiredVideoHeight));
      const desiredWidth = clampedVideoHeight * 1.18;
      const cappedWidth = Math.min(Math.max(desiredWidth, 420), 680);

      document.documentElement.style.setProperty("--center-col-max", cappedWidth.toFixed(1) + "px");
    }

    function updateMetric(prefix, index, value, min, max) {
      const fill = document.getElementById(prefix + "-fill-" + index);
      const label = document.getElementById(prefix + "-value-" + index);
      const clamped = Math.max(min, Math.min(max, value));
      const percent = ((clamped - min) / (max - min)) * 100;
      fill.style.width = percent.toFixed(1) + "%";
      label.textContent = Math.round(value);
    }

    function setUltrasonicDisabled(index) {
      const row = document.getElementById("ult-row-" + index);
      const fill = document.getElementById("ult-fill-" + index);
      const label = document.getElementById("ult-value-" + index);

      row.classList.add("metric-disabled");
      fill.style.width = "0%";
      label.textContent = "--";
    }

    function formatUltrasonicValue(pwUs) {
      const cm = pwUs / 58.0;
      const inches = pwUs / 148.0;

      switch (ultDisplayUnit) {
        case "m":
          return {
            valueText: (cm / 100.0).toFixed(2),
            barValue: cm / 100.0,
            barMin: 0,
            barMax: 2
          };
        case "ftin": {
          const feet = Math.floor(inches / 12.0);
          const remInches = inches - feet * 12.0;
          return {
            valueText: feet + "ft " + remInches.toFixed(1) + "in",
            barValue: cm,
            barMin: 0,
            barMax: 200
          };
        }
        case "pw":
          return {
            valueText: Math.round(pwUs) + " us",
            barValue: pwUs,
            barMin: 0,
            barMax: 20000
          };
        case "cm":
        default:
          return {
            valueText: cm.toFixed(1),
            barValue: cm,
            barMin: 0,
            barMax: 200
          };
      }
    }

    function renderUltrasonicMetric(index, pwUs, enabled) {
      const row = document.getElementById("ult-row-" + index);
      const fill = document.getElementById("ult-fill-" + index);
      const label = document.getElementById("ult-value-" + index);

      if (!enabled) {
        setUltrasonicDisabled(index);
        return;
      }

      row.classList.remove("metric-disabled");

      const display = formatUltrasonicValue(pwUs);
      const clamped = Math.max(display.barMin, Math.min(display.barMax, display.barValue));
      const percent = ((clamped - display.barMin) / (display.barMax - display.barMin)) * 100;
      const cm = pwUs / 58.0;

      fill.style.width = percent.toFixed(1) + "%";
      fill.style.background = colorForUltrasonic(cm);
      label.textContent = display.valueText;
    }

    function updateNamedMetric(name, value, min, max) {
      const fill = document.getElementById(name.toLowerCase() + "-fill");
      const label = document.getElementById(name.toLowerCase() + "-value");
      const clamped = Math.max(min, Math.min(max, value));
      const percent = ((clamped - min) / (max - min)) * 100;
      fill.style.width = percent.toFixed(1) + "%";
      label.textContent = value.toFixed(1);
    }

    function updateCenteredMetricFormatted(name, value, neutral, span, labelText) {
      const fill = document.getElementById(name.toLowerCase() + "-fill");
      const label = document.getElementById(name.toLowerCase() + "-value");
      const row = document.getElementById(name.toLowerCase() + "-row");
      const normalized = Math.max(-1, Math.min(1, (value - neutral) / span));
      const width = Math.abs(normalized) * 50;
      const magnitude = Math.abs(normalized);
      const startLightness = 28 + magnitude * 10;
      const endLightness = 48 + magnitude * 10;

      row.classList.remove("metric-disabled");

      if (normalized >= 0) {
        fill.style.left = "50%";
        fill.style.borderRadius = "0 999px 999px 0";
      } else {
        fill.style.left = (50 - width) + "%";
        fill.style.borderRadius = "999px 0 0 999px";
      }

      fill.style.width = width + "%";
      fill.style.background =
        "linear-gradient(90deg, hsl(28 48% " + startLightness.toFixed(1) +
        "%), hsl(28 74% " + endLightness.toFixed(1) + "%))";
      label.textContent = labelText;
    }

    function updateCenteredMetric(name, value, neutral, span) {
      updateCenteredMetricFormatted(name, value, neutral, span, Math.round(value));
    }

    function colorForUltrasonic(cm) {
      if (cm < 35) {
        return "linear-gradient(90deg, #a82c2c, #ef5350)";
      }
      if (cm < 90) {
        return "linear-gradient(90deg, #a65c1c, #f0a43a)";
      }
      return "linear-gradient(90deg, #4f9a35, #9fe870)";
    }

    function colorForBattery(volts) {
      if (volts < 11.0) {
        return "linear-gradient(90deg, #a82c2c, #ef5350)";
      }
      if (volts < 11.8) {
        return "linear-gradient(90deg, #a65c1c, #f0a43a)";
      }
      return "linear-gradient(90deg, #4f9a35, #9fe870)";
    }

    function formatStamp() {
      return new Date().toLocaleTimeString([], { hour12: false });
    }

    function applyTelemetry(snapshot) {
      latestTelemetry = snapshot;

      for (let i = 0; i < 6; i += 1) {
        updateCenteredMetric("RC" + i, snapshot.rc[i], 1500, 500);
      }

      for (let i = 0; i < 10; i += 1) {
        renderUltrasonicMetric(i, snapshot.ult[i], !!snapshot.ult_en[i]);
      }

      updateCenteredMetric("MTR0", snapshot.mtr[0], 1500, 500);
      updateCenteredMetric("MTR1", snapshot.mtr[1], 1500, 500);
      updateCenteredMetric("PAN", snapshot.srv[0], 1500, 500);
      updateCenteredMetric("TILT", snapshot.srv[1], 1500, 500);

      if (snapshot.imu_ok) {
        updateCenteredMetricFormatted("AX", snapshot.imu_accel_mg[0] / 1000.0, 0, 2,
          (snapshot.imu_accel_mg[0] / 1000.0).toFixed(3));
        updateCenteredMetricFormatted("AY", snapshot.imu_accel_mg[1] / 1000.0, 0, 2,
          (snapshot.imu_accel_mg[1] / 1000.0).toFixed(3));
        updateCenteredMetricFormatted("AZ", snapshot.imu_accel_mg[2] / 1000.0, 0, 2,
          (snapshot.imu_accel_mg[2] / 1000.0).toFixed(3));
        updateCenteredMetricFormatted("GX", snapshot.imu_gyro_mdps[0] / 1000.0, 0, 250,
          (snapshot.imu_gyro_mdps[0] / 1000.0).toFixed(2));
        updateCenteredMetricFormatted("GY", snapshot.imu_gyro_mdps[1] / 1000.0, 0, 250,
          (snapshot.imu_gyro_mdps[1] / 1000.0).toFixed(2));
        updateCenteredMetricFormatted("GZ", snapshot.imu_gyro_mdps[2] / 1000.0, 0, 250,
          (snapshot.imu_gyro_mdps[2] / 1000.0).toFixed(2));
      } else {
        ["AX","AY","AZ","GX","GY","GZ"].forEach(function(name) {
          setCenteredMetricUnavailable(name);
        });
      }

      const encValid = Array.isArray(snapshot.enc_valid)
        ? snapshot.enc_valid
        : [!!snapshot.enc_valid, !!snapshot.enc_valid];
      for (let i = 0; i < 2; i += 1) {
        const name = "ENC" + i;
        updateCenteredMetricFormatted(name, snapshot.enc[i], 0, 32768, Math.round(snapshot.enc[i]));
        if (!encValid[i]) {
          document.getElementById(name.toLowerCase() + "-row").classList.add("metric-disabled");
        }
      }

      const batteryValue = snapshot.bat_mv / 1000.0;
      updateNamedMetric("BAT", batteryValue, 10.5, 12.6);
      document.getElementById("bat-fill").style.background = colorForBattery(batteryValue);

      setTelemetryStamp("Last Update " + formatStamp());
      window.requestAnimationFrame(tuneCenterColumnWidth);
    }

    ultUnitSelect.addEventListener("change", function() {
      ultDisplayUnit = ultUnitSelect.value;
      if (latestTelemetry) {
        applyTelemetry(latestTelemetry);
      }
    });

    async function pollTelemetry() {
      if (telemetryInFlight) {
        return;
      }

      telemetryInFlight = true;
      try {
        const response = await fetch("/telemetry", { cache: "no-store" });
        if (!response.ok) {
          throw new Error("HTTP " + response.status);
        }

        const snapshot = await response.json();
        if (snapshot.connected) {
          applyTelemetry(snapshot);
          if (!telemetryOnline) {
            logLine("[link] MSP telemetry online");
          }
          telemetryOnline = true;
        } else {
          if (telemetryOnline) {
            logLine("[warn] MSP telemetry timed out");
          }
          telemetryOnline = false;
          setTelemetryStamp("Last Update --:--:--");
        }
      } catch (error) {
        if (telemetryOnline) {
          logLine("[warn] telemetry poll lost");
        }
        telemetryOnline = false;
        setTelemetryStamp("Last Update --:--:--");
      } finally {
        telemetryInFlight = false;
      }
    }

    async function pollConsole() {
      if (consoleInFlight) {
        return;
      }

      consoleInFlight = true;
      try {
        const response = await fetch("/console?since=" + consoleSeq, { cache: "no-store" });
        if (!response.ok) {
          return;
        }

        const payload = await response.json();
        consoleSeq = payload.next || consoleSeq;
        if (Array.isArray(payload.lines)) {
          payload.lines.forEach(function(line) {
            logLine(line);
          });
        }
      } catch (error) {
      } finally {
        consoleInFlight = false;
      }
    }

    function setStreamStatus(text, color, pulse) {
      bridgeStatus.textContent = text;
      bridgeStatus.style.color = color;
      bridgeStatus.style.setProperty("--pill-color", color);
      bridgeStatus.style.setProperty("--pill-ring", color === "#9fe870" ? "rgba(159, 232, 112, 0.12)" : "rgba(147, 160, 173, 0.10)");
      bridgeStatus.style.setProperty("--pill-pulse", pulse ? "pulse 1.8s infinite" : "none");
    }

    function clearStreamTimers() {
      if (streamTimer !== null) {
        window.clearTimeout(streamTimer);
        streamTimer = null;
      }
      if (streamTimeout !== null) {
        window.clearTimeout(streamTimeout);
        streamTimeout = null;
      }
      if (streamFrame !== null) {
        streamFrame.onload = null;
        streamFrame.onerror = null;
        streamFrame.src = "";
      }
      streamRequestInFlight = false;
      streamFrame = null;
    }

    function requestSnapshot() {
      if (!streamEnabled || streamRequestInFlight) {
        return;
      }

      streamRequestInFlight = true;
      streamRevision += 1;
      const nextFrame = new Image();
      streamFrame = nextFrame;

      nextFrame.onload = function() {
        if (streamFrame !== nextFrame) {
          return;
        }

        streamRequestInFlight = false;
        streamFrame = null;
        if (streamTimeout !== null) {
          window.clearTimeout(streamTimeout);
          streamTimeout = null;
        }

        if (streamEnabled) {
          streamImg.src = nextFrame.src;
          setStreamStatus("Snapshot Video", "#9fe870", true);
          streamTimer = window.setTimeout(requestSnapshot, snapshotPollMs);
        }
      };

      nextFrame.onerror = function() {
        if (streamFrame !== nextFrame) {
          return;
        }

        streamRequestInFlight = false;
        streamFrame = null;
        if (streamTimeout !== null) {
          window.clearTimeout(streamTimeout);
          streamTimeout = null;
        }

        if (streamEnabled) {
          setStreamStatus("Recovering Video", "#93a0ad", false);
          streamTimer = window.setTimeout(requestSnapshot, snapshotPollMs);
        }
      };

      streamTimeout = window.setTimeout(function() {
        if (streamFrame !== nextFrame) {
          return;
        }

        nextFrame.onload = null;
        nextFrame.onerror = null;
        nextFrame.src = "";
        streamRequestInFlight = false;
        streamFrame = null;
        if (streamEnabled) {
          setStreamStatus("Recovering Video", "#93a0ad", false);
          streamTimer = window.setTimeout(requestSnapshot, snapshotPollMs);
        }
      }, snapshotTimeoutMs);

      nextFrame.src = streamUrl + "?v=" + streamRevision;
    }

    function scheduleNextSnapshot() {
      if (streamTimeout !== null) {
        window.clearTimeout(streamTimeout);
        streamTimeout = null;
      }
      if (streamEnabled) {
        streamTimer = window.setTimeout(requestSnapshot, snapshotPollMs);
      }
    }

    function applyStreamState() {
      clearStreamTimers();

      if (streamEnabled) {
        streamImg.style.display = "block";
        streamToggle.textContent = "Stop Stream";
        setStreamStatus("Snapshot Video", "#9fe870", true);
        requestSnapshot();
      } else {
        streamImg.src = "";
        streamImg.removeAttribute("src");
        streamImg.style.display = "none";
        streamToggle.textContent = "Start Stream";
        setStreamStatus("Stream Paused", "#93a0ad", false);
      }
    }

    function connectStream() {
      streamHost.textContent = streamUrl;
      applyStreamState();
    }

    commandForm.addEventListener("submit", async function(event) {
      event.preventDefault();
      const command = commandInput.value.trim();
      if (!command) {
        return;
      }

      try {
        const response = await fetch("/command?cmd=" + encodeURIComponent(command), { cache: "no-store" });
        const result = await response.json();
        logLine((result.accepted ? "[tx] " : "[err] ") + result.message);
      } catch (error) {
        logLine("[err] command transport unavailable");
      } finally {
        commandInput.value = "";
      }
    });

    streamToggle.addEventListener("click", function() {
      streamEnabled = !streamEnabled;
      applyStreamState();
    });

    connectStream();
    pollTelemetry();
    pollConsole();
    tuneCenterColumnWidth();
    window.addEventListener("resize", tuneCenterColumnWidth);
    telemetryTimer = window.setInterval(pollTelemetry, telemetryPollMs);
    window.setInterval(pollConsole, consolePollMs);
  </script>
</body>
</html>
)rawliteral";

#endif
