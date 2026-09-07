<div align="center">

# 🦈 SHARK AntiCheat

**A Windows-based anti-cheat monitoring toolkit built around `HD-Player.exe`**

[![Platform](https://img.shields.io/badge/platform-Windows-0078D6?logo=windows&logoColor=white)](#)
[![Language](https://img.shields.io/badge/language-C%2B%2B17-00599C?logo=cplusplus&logoColor=white)](#)
[![Status](https://img.shields.io/badge/status-active-brightgreen)](#)
[![Discord](https://img.shields.io/badge/Discord-Join%20Server-5865F2?logo=discord&logoColor=white)](https://discord.gg/CHYhSH3sKB)

</div>

---

## 📦 Versions

| Version | Status | Description | Link |
|---|---|---|---|
| **v1.0.0** | ✅ Stable | Full detection suite — process, memory, module, driver, debugger, and environment checks with Discord webhook reporting | [Open folder →](./Shark%20Anticheat) |
| **v2.0.0** | 🚧 In development | Next-generation rebuild with `SHARK_Guardian.cpp` companion watchdog and improved build scripts | [Open folder →](./Shark%20Anticheat%20v2) |

---

## 🛡️ What is SHARK AntiCheat?

SHARK AntiCheat is a monitoring tool designed to detect tampering, injection, and cheating attempts against `HD-Player.exe`. It combines static checks, runtime memory analysis, behavioral heuristics, and environment detection, then reports every event live through **Discord webhooks**.

> ⚠️ **Before you compile or distribute:** this project uses privileged access, hardware identification, Discord telemetry, and Guardian executable deployment. Review the security and privacy implications for your use case first.

---

## ✨ Highlights (v1.0.0)

- 🔍 Unsigned executable/module & Authenticode verification
- 🧩 Injected module and suspicious thread detection
- 🧠 Manual-mapped DLL and memory integrity checks
- 🐞 Debugger attachment detection
- 🖥️ VM environment & screen-recorder detection
- 🎯 Aim-anomaly and time-manipulation heuristics
- 💓 Runtime heartbeat + tamper-resistant watchdog (Guardian)

Full detection matrix is documented in the [v1.0.0 README](./Shark%20Anticheat/Readme.md).

---

## 🚀 Getting Started

Both versions are written in C++ and compiled with MinGW (`g++`) on Windows.

```cmd
g++ SHARK_AntiCheat.cpp -o SHARK_AntiCheat.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
g++ SHARK_Guardian.cpp -o SHARK_Guardian.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
```

📘 Full step-by-step build guide (MinGW setup, PATH configuration, troubleshooting): [`how_to_convert_cpp_to_exe.md`](./Shark%20Anticheat/how_to_convert_cpp_to_exe.md)

Or just run the included build scripts:

```cmd
build_shark_anticheat.bat
build_shark_guardian.bat
```

---

## 📁 Repository Structure

```
Shark_Anticheat/
├── Shark Anticheat/              # v1.0.0 — stable release
│   ├── SHARK_AntiCheat.cpp
│   ├── SHARK_Guardian.cpp
│   ├── build_shark_anticheat.bat
│   ├── build_shark_guardian.bat
│   ├── how_to_convert_cpp_to_exe.md
│   └── Readme.md
│
└── Shark Anticheat v2/           # v2.0.0 — in development
    ├── SHARK_AntiCheat.cpp
    ├── SHARK_Guardian.cpp
    ├── build_shark_anticheat.bat
    └── build_shark_guardian.bat
```

---

## 💬 Support

Questions, bugs, or feature requests — join the Discord:

**[👉 discord.gg/CHYhSH3sKB](https://discord.gg/CHYhSH3sKB)**

---

<div align="center">

*Last updated: 2026*

</div>



<div align="center">

# 🦈 SHARK AntiCheat — v2.0.0

**Next-generation rebuild of SHARK AntiCheat**

[![Status](https://img.shields.io/badge/status-in%20development-orange)](#)
[![Platform](https://img.shields.io/badge/platform-Windows-0078D6?logo=windows&logoColor=white)](#)
[![Discord](https://img.shields.io/badge/Discord-Join%20Server-5865F2?logo=discord&logoColor=white)](https://discord.gg/CHYhSH3sKB)

</div>

---

## 🚧 Status

Version 2.0.0 is currently **under active development**. The build pipeline (`.bat` scripts) is in place; the detection engine (`SHARK_AntiCheat.cpp`, `SHARK_Guardian.cpp`) is being rebuilt from the ground up and will be published here as it lands.

Looking for the working release right now? Head to **[v1.0.0](../Shark%20Anticheat)**.

---

## 📁 Contents

| File | Purpose |
|---|---|
| `SHARK_AntiCheat.cpp` | Core detection engine *(in progress)* |
| `SHARK_Guardian.cpp` | Watchdog companion process *(in progress)* |
| `build_shark_anticheat.bat` | Compiles `SHARK_AntiCheat.cpp` with MinGW |
| `build_shark_guardian.bat` | Compiles `SHARK_Guardian.cpp` with MinGW |

---

## 🛠️ Building

Once the source files are populated, compile with:

```cmd
build_shark_anticheat.bat
build_shark_guardian.bat
```

These scripts call `g++` with the required Windows libraries (`psapi`, `wbemuuid`, `wininet`, `shell32`, `advapi32`, `wintrust`, `crypt32`, `ole32`, `oleaut32`, `uuid`, `ws2_32`) using the C++17 standard with `-O2` optimization.

For a full manual build walkthrough, see the [v1.0.0 build guide](../Shark%20Anticheat/how_to_convert_cpp_to_exe.md) — the same MinGW setup applies here.

---

## 💬 Follow Progress / Get Support

Join the Discord to follow v2.0.0 development or ask questions:

**[👉 discord.gg/CHYhSH3sKB](https://discord.gg/CHYhSH3sKB)**

---

<div align="center">

*Last updated: 2026*

</div>
