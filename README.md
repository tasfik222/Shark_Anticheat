# 🦈 SHARK AntiCheat

**A Windows-based anti-cheat monitoring toolkit built around `HD-Player.exe`**

[![Platform](https://img.shields.io/badge/platform-Windows-0078D6?logo=windows&logoColor=white)](#)
[![Language](https://img.shields.io/badge/language-C%2B%2B17-00599C?logo=cplusplus&logoColor=white)](#)
[![Status](https://img.shields.io/badge/status-stable-brightgreen)](#)
[![Discord](https://img.shields.io/badge/Discord-Join%20Server-5865F2?logo=discord&logoColor=white)](https://discord.gg/CHYhSH3sKB)

---

## 📦 Versions

| Version | Codename | Status | Description | Link |
|---|---|---|---|---|
| **v1.0.0** | Shark AntiCheat | ✅ Stable | Full detection suite — process, memory, module, driver, debugger, and environment checks with Discord webhook reporting | [Open folder →](https://github.com/tasfik222/Shark_Anticheat/tree/main/Shark%20Anticheat) |
| **v2.0.0** | 🔴 Red Eye AntiCheat | ✅ Stable | Next-generation rebuild with `SHARK_Guardian.cpp` companion watchdog and improved build scripts | [Open folder →](https://github.com/tasfik222/Shark_Anticheat/tree/main/Red%20Eye%20Anticheat) |

> ℹ️ **Note:** Version 2.0.0 has been rebranded as **Red Eye AntiCheat** and now lives in the `Red Eye Anticheat/` folder (previously referred to as "Shark Anticheat v2" — that name/folder no longer exists and any old links to it are broken).

---

## 🛡️ What is SHARK AntiCheat?

SHARK AntiCheat is a monitoring tool designed to detect tampering, injection, and cheating attempts against `HD-Player.exe`. It combines static checks, runtime memory analysis, behavioral heuristics, and environment detection, then reports every event live through **Discord webhooks**.

> ⚠️ **Before you compile or distribute:** this project uses privileged access, hardware identification, Discord telemetry, and Guardian executable deployment. Review the security and privacy implications for your use case first. **Never hardcode your Discord webhook URL directly in a public source file** — use an environment variable or a local, git-ignored config file instead.

---

## ✨ Highlights (v1.0.0 — Shark AntiCheat)

- 🔍 Unsigned executable/module & Authenticode verification
- 🧩 Injected module and suspicious thread detection
- 🧠 Manual-mapped DLL and memory integrity checks
- 🐞 Debugger attachment detection
- 🖥️ VM environment & screen-recorder detection
- 🎯 Aim-anomaly and time-manipulation heuristics
- 💓 Runtime heartbeat + tamper-resistant watchdog (Guardian)

Full detection matrix is documented in the [v1.0.0 README](https://github.com/tasfik222/Shark_Anticheat/blob/main/Shark%20Anticheat/Readme.md).

---

## 🔴 About Red Eye AntiCheat (v2.0.0)

Red Eye AntiCheat is the ground-up rebuild of the detection engine, now **stable and ready to use**. It builds on the v1.0.0 detection matrix with a rewritten core (`SHARK_AntiCheat.cpp`) and companion watchdog (`SHARK_Guardian.cpp`), plus improved build scripts.

Both **v1.0.0 (Shark AntiCheat)** and **v2.0.0 (Red Eye AntiCheat)** are currently maintained. If you're starting fresh, **Red Eye AntiCheat is the recommended version**.

---

## 🚀 Getting Started

Both versions are written in C++ and compiled with MinGW (`g++`) on Windows.

```bash
g++ SHARK_AntiCheat.cpp -o SHARK_AntiCheat.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
g++ SHARK_Guardian.cpp -o SHARK_Guardian.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
```

📘 Full step-by-step build guide (MinGW setup, PATH configuration, troubleshooting):
[`how_to_convert_cpp_to_exe.md`](https://github.com/tasfik222/Shark_Anticheat/blob/main/Shark%20Anticheat/how_to_convert_cpp_to_exe.md)

Or just run the included build scripts:

```bash
build_shark_anticheat.bat
build_shark_guardian.bat
```

**Linked libraries:**

| Library | Purpose |
|---|---|
| `ole32` | COM (Component Object Model) support |
| `oleaut32` | OLE Automation functions |
| `wininet` | Internet/HTTP request handling |
| `wintrust` | File signature / Authenticode verification |
| `wbemuuid` | WMI (Windows Management Instrumentation) queries |

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
└── Red Eye Anticheat/            # v2.0.0 — stable release (formerly "Shark Anticheat v2")
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

*Last updated: 2026*
