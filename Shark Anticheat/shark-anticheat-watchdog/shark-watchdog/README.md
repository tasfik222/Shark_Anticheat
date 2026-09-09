# SHARK-AntiCheat Watchdog

A tiny, console-free background tool that watches **SHARK-AntiCheat.exe**
(name configurable) and alerts the moment it detects:

1. **The process being killed** — it disappears from the process list.
2. **A thread being killed** — an individual thread inside it disappears
   while the process itself keeps running (selective thread-kill attack).
3. **A thread being suspended** — a thread's state flips to
   Waiting/Suspended (the classic `SuspendThread`-based attack).
4. **The whole process being suspended** — every thread suspended at once
   (an `NtSuspendProcess`-style freeze, which doesn't show up as
   termination at all, so a naive "is the process still running?" check
   would miss it).

It's **read-only**: it only opens things with query/enumerate rights and
never writes to, kills, resumes, or otherwise touches the target process.
Alerts go to a local log file (always) and to a Discord webhook (if you
configure one).

No console window ever appears — it's built with the Windows GUI
subsystem, so double-clicking the exe just starts it silently in the
background (you'll see it in Task Manager, nothing else).

## Files

```
src/main.cpp              all the code (single file, deliberately — easy
                           to build with just a compiler, no project setup)
third_party/json.hpp       vendored nlohmann/json (single header)
watchdog_config.json       config template — copy next to the built exe
```

## Build

### Option A — Visual Studio (recommended on a real Windows box)

Open a **Developer Command Prompt for VS** and run:

```powershell
cl /std:c++17 /EHsc /O2 /DUNICODE /D_UNICODE src\main.cpp /link /SUBSYSTEM:WINDOWS winhttp.lib /out:SHARK-Watchdog.exe
```

Or create a new "Windows Desktop Application" (not Console App) project in
Visual Studio, add `src/main.cpp`, and add `winhttp.lib` under
**Project Properties → Linker → Input → Additional Dependencies**.

### Option B — MinGW-w64 (g++)

```powershell
g++ -std=c++17 -O2 -municode -mwindows -o SHARK-Watchdog.exe src\main.cpp -lwinhttp
```

`-mwindows` is what makes it a GUI-subsystem binary with no console.
`-municode` makes `wWinMain` the entry point Windows expects for a
Unicode GUI app.

This exact command (cross-compiled) is what was used to validate the code
compiles cleanly against real Windows SDK headers before handing it to
you — confirmed output: `PE32+ executable (GUI) x86-64, for MS Windows`.

## Deploy

1. Put `SHARK-Watchdog.exe` and `watchdog_config.json` in the **same
   folder**.
2. Edit `watchdog_config.json`:
   - `target_process_name` — exact process image name to watch (default
     `SHARK-AntiCheat.exe`)
   - `discord_webhook` — your webhook URL, or leave the placeholder to
     run in log-only mode
   - `poll_interval_ms` — how often to snapshot (default 1000ms/1s;
     lower = faster detection, marginally more CPU)
3. Run `SHARK-Watchdog.exe`. No window opens — check `watchdog.log` in
   the same folder to confirm it started ("Watchdog started...").
4. To have it start automatically: put a shortcut to the exe in
   `shell:startup`, or register it as a Scheduled Task set to run at
   logon (more reliable than the Startup folder, and can be set to run
   with highest privileges).

If `watchdog_config.json` is missing, it falls back to built-in defaults
(target = `SHARK-AntiCheat.exe`, poll = 1000ms, no Discord webhook,
log-only) rather than failing to start.

## How detection actually works (so you can trust/debug it)

- **Process presence**: `CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS)`
  each cycle, looking for the target name. Gone from the list = alert.
- **Per-thread state**: `NtQuerySystemInformation(SystemProcessInformation)`
  — the same technique Process Hacker / System Informer use to show a
  thread as "Suspended" in their UI without opening the thread directly.
  Each thread's `ThreadState`/`WaitReason` fields tell us if it's
  currently parked in a suspended wait state. This is compared tid-by-tid
  against the previous snapshot to catch new suspensions, resumes, and
  threads that vanished outright.
- **Whole-process suspension**: if 100% of currently-known threads are
  suspended at the same time (and none of them were just killed), that's
  flagged as the process being suspended as a whole, separately from any
  individual thread-suspend alert.

## Honest limitations

- **Polling, not event-driven.** At the default 1s interval, an attacker
  who suspends the process for under a second and resumes it before the
  next poll could theoretically slip through undetected. Lower
  `poll_interval_ms` if you need tighter coverage (500ms is still cheap).
- **Doesn't distinguish attacker vs. legitimate suspension.** Some tools
  (debuggers, some anti-cheat's own internal checks, certain crash
  handlers) legitimately suspend threads briefly. Treat alerts as "worth
  a human glance," same guidance as the main tournament monitor.
- **Watchdog itself isn't hardened against being killed/suspended in
  turn.** This tool has no self-protection built in (no anti-debug, no
  process protection). If you need "who watches the watchdog," the usual
  answer is running two independent watchdogs that each monitor the
  other, or wrapping this in a Windows Service (services are a bit
  harder for a casual attacker to just Task-Manager-kill, though not
  impossible with sufficient privileges). Happy to build either of those
  next if useful.
