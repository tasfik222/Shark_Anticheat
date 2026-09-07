SHARK AntiCheat — Detection Overview
SHARK_AntiCheat.cpp is a Windows monitoring tool designed primarily around HD-Player.exe. It performs process, memory, module, driver, debugger, and environment checks, then reports events through Discord webhook messages.

Note: This project uses privileged access, hardware identification, Discord telemetry, and Guardian executable deployment. Review the security and privacy implications before compiling or distributing it.

# 🛡️ Anti-Cheat Detection Features

An overview of the detection mechanisms implemented by this anti-cheat system. Each entry lists the **feature**, the **detection method**, and the **reported result** when a violation is found.
Avoid the 5th number (cheat memory signatures) in the update. The updated version will provide real detection of cheat memory signatures.

---

## 📋 Detection Matrix

| # | Feature | Detection Method | Reported Result |
|---|---------|-------------------|------------------|
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
| 15 | **Thread tampering** | Watchdog checks whether monitoring threads were suspended or terminated | Reports and attempts to resume suspended threads |
| 16 | **Runtime heartbeat** | Periodically posts running status | Reports that the tool remains active |

---

## 🧩 Summary

This system combines **static checks** (signature verification, module enumeration), **runtime memory analysis** (signature scanning, integrity checksums), **behavioral heuristics** (aim anomaly, time manipulation), and **environment checks** (VM detection, screen-recording tools) to provide layered protection against tampering, injection, and cheating.

---

*Last updated: 2026*
