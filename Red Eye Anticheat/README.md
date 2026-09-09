🔴 Red Eye AntiCheat — Detection Overview
==========================================

`SHARK_AntiCheat.cpp` (Red Eye build) is a Windows monitoring tool designed primarily around `HD-Player.exe`. It is the next-generation rebuild of Shark AntiCheat v1.0.0, performing process, memory, module, driver, debugger, and environment checks, then reporting events through Discord webhook messages, backed by a companion watchdog (`SHARK_Guardian.cpp`).

> **Note:** This project uses privileged access, hardware identification, Discord telemetry, and Guardian executable deployment. Review the security and privacy implications before compiling or distributing it. Never hardcode your Discord webhook URL directly in source — use an environment variable or a local, git-ignored config file instead.

---

## 🛡️ Anti-Cheat Detection Features

An overview of the detection mechanisms implemented by this anti-cheat system. Each entry lists the **feature**, the **detection method**, and the **reported result** when a violation is found.

---

## 📋 Detection Matrix

| # | Feature | Detection Method | Reported Result |
|---|---|---|---|
| 1 | **Unsigned executable/module** | Verifies Authenticode signatures with `WinVerifyTrust` | Flags unsigned EXEs and DLLs |
| 2 | **Unsigned/injected modules** | Enumerates loaded modules in the target process | Sends module name and path alerts |
| 3 | **Suspicious threads** | Checks whether thread start addresses fall outside known module ranges | Flags possible injected or unknown threads |
| 4 | **Manual-mapped DLLs** | Looks for committed, private executable RWX memory regions with an MZ header | Flags possible hidden/manual-mapped PE images |
| 5 | **Cheat memory signatures** | Searches target-process memory for hard-coded byte patterns | Detects markers such as `CheatEngine7`, `AimbotEnable`, `ESP_ENABLE`, `InjectedDLL`, and NOP sleds |
| 6 | **Debugger attachment** | Uses `CheckRemoteDebuggerPresent` on `HD-Player.exe` | Flags an attached debugger |
| 7 | **Memory integrity changes** | Stores checksums of main-module memory pages and compares them later | Reports changed memory regions |
| 8 | **External process handles** | Enumerates system handles and identifies processes holding a handle to `HD-Player.exe` | Flags and attempts to close suspicious handles |
| 9 | **Kernel drivers** | Enumerates active driver services and checks their names against keywords | Flags drivers containing terms such as `aimbot`, `wallhack`, `injector`, or `loader` |
| 10 | **Screen capture tools** | Checks active process names against a recording-tool list | Detects OBS, Fraps, Bandicam, XSplit, Medal, Outplayed, and similar tools |
| 11 | **VM environment** | Checks VMware/VirtualBox registry artifacts and processes | Flags a possible virtual-machine environment |
| 12 | **Aim anomaly heuristic** | Measures rapid cursor movement and counts high-speed "snap" events | Reports potential aimbot-like movement |
| 13 | **Time manipulation** | Compares expected and actual timing intervals | Reports possible time tampering or speed-hack behavior |
| 14 | **Overlay window check** | Searches target-process windows for a specific style value | Attempts to close a suspected ESP overlay window |
| 15 | **Thread tampering (Guardian)** | Watchdog checks whether monitoring threads were suspended or terminated | Reports and attempts to resume suspended threads |
| 16 | **Runtime heartbeat** | Periodically posts running status | Reports that the tool remains active |

> ⚠️ These are the carried-over detections inherited from Shark AntiCheat v1.0.0. If the Red Eye rebuild has added, removed, or changed any detection module, update this table to match the actual `SHARK_AntiCheat.cpp` / `SHARK_Guardian.cpp` source before publishing — this file should always reflect the real code, not the previous version's assumptions.

---

## 🧩 Summary

This system combines **static checks** (signature verification, module enumeration), **runtime memory analysis** (signature scanning, integrity checksums), **behavioral heuristics** (aim anomaly, time manipulation), and **environment checks** (VM detection, screen-recording tools) to provide layered protection against tampering, injection, and cheating.

---

## 🚀 Build Instructions

```bash
g++ SHARK_AntiCheat.cpp -o SHARK_AntiCheat.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
g++ SHARK_Guardian.cpp -o SHARK_Guardian.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
```

Or run the included build scripts:

```bash
build_shark_anticheat.bat
build_shark_guardian.bat
```

For a full manual build walkthrough (MinGW setup, PATH configuration, troubleshooting), see the [Shark AntiCheat v1.0.0 build guide](https://github.com/tasfik222/Shark_Anticheat/blob/main/Shark%20Anticheat/how_to_convert_cpp_to_exe.md) — the same MinGW setup applies here.

---

## 📁 Contents

| File | Purpose |
|---|---|
| `SHARK_AntiCheat.cpp` | Core detection engine |
| `SHARK_Guardian.cpp` | Watchdog companion process |
| `build_shark_anticheat.bat` | Compiles `SHARK_AntiCheat.cpp` with MinGW |
| `build_shark_guardian.bat` | Compiles `SHARK_Guardian.cpp` with MinGW |

---

Need any help, [join my discord server](https://discord.gg/CHYhSH3sKB)

*Last updated: 2026*
