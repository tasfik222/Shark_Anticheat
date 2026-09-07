SHARK AntiCheat — Detection Overview
SHARK_AntiCheat.cpp is a Windows monitoring tool designed primarily around HD-Player.exe. It performs process, memory, module, driver, debugger, and environment checks, then reports events through Discord webhook messages.

Note: This project uses privileged access, hardware identification, Discord telemetry, and Guardian executable deployment. Review the security and privacy implications before compiling or distributing it.

[Feature-DetectionMethod-ReportedResult.csv](https://github.com/user-attachments/files/31916852/Feature-DetectionMethod-ReportedResult.csv)
"Feature","Detection Method","Reported Result"
"Unsigned executable/module","Verifies Authenticode signatures with WinVerifyTrust","Flags unsigned EXEs and DLLs"
"Unsigned/injected modules","Enumerates loaded modules in the target process","Sends module name and path alerts"
"Suspicious threads","Checks whether thread start addresses fall outside known module ranges","Flags possible injected or unknown threads"
"Manual-mapped DLLs","Looks for committed, private executable RWX memory regions with an MZ header","Flags possible hidden/manual-mapped PE images"
"Cheat memory signatures","Searches target-process memory for hard-coded byte patterns","Detects markers such as CheatEngine7, AimbotEnable, ESP_ENABLE, InjectedDLL, and NOP sleds"
"Debugger attachment","Uses CheckRemoteDebuggerPresent on HD-Player.exe","Flags an attached debugger"
"Memory integrity changes","Stores checksums of main-module memory pages and compares them later","Reports changed memory regions"
"External process handles","Enumerates system handles and identifies processes holding a handle to HD-Player.exe","Flags and attempts to close suspicious handles"
"Kernel drivers","Enumerates active driver services and checks their names against keywords","Flags drivers containing terms such as aimbot, wallhack, injector, or loader"
"Screen capture tools","Checks active process names against a recording-tool list","Detects OBS, Fraps, Bandicam, XSplit, Medal, Outplayed, and similar tools"
"VM environment","Checks VMware/VirtualBox registry artifacts and processes","Flags a possible virtual-machine environment"
"Aim anomaly heuristic","Measures rapid cursor movement and counts high-speed “snap” events","Reports potential aimbot-like movement"
"Time manipulation","Compares expected and actual timing intervals","Reports possible time tampering or speed-hack behavior"
"Overlay window check","Searches target-process windows for a specific style value","Attempts to close a suspected ESP overlay window"
"Thread tampering","Watchdog checks whether monitoring threads were suspended or terminated","Reports and attempts to resume suspended threads"
"Runtime heartbeat","Periodically posts running status","Reports that the tool remains active"
