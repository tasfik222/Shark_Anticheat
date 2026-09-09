// SHARK-AntiCheat Watchdog
// ------------------------
// A console-free (GUI-subsystem) background monitor that watches a target
// process by name -- default "SHARK-AntiCheat.exe" -- and raises an alert
// (Discord webhook + local log file) the moment it detects:
//
//   1. The target process disappearing (killed / crashed / exited)
//   2. Any of its threads disappearing (individually killed)
//   3. Any of its threads being suspended (SuspendThread-style attack)
//   4. The ENTIRE process appearing suspended (NtSuspendProcess-style
//      "freeze it instead of killing it" attack, which doesn't show up
//      as termination at all)
//
// This is a read-only watchdog: it never opens the target with anything
// beyond query/synchronize rights, never writes to it, and never resumes
// or kills anything itself -- it only observes and reports, exactly like
// the earlier tournament monitor. Detection method: periodic snapshots via
// CreateToolhelp32Snapshot (process/thread enumeration) and
// NtQuerySystemInformation(SystemProcessInformation) for per-thread
// state/wait-reason (the same technique Process Hacker/System Informer
// uses to show a thread as "Suspended" without opening it directly).
//
// No console window: built as a Windows-subsystem (GUI) executable via
// WinMain, so double-clicking it just runs silently in the background
// (visible only in Task Manager). All output goes to the log file and,
// if configured, a Discord webhook.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <winhttp.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <mutex>
#include <thread>
#include <chrono>
#include <ctime>
#include <cstdio>

#include "../third_party/json.hpp"

#pragma comment(lib, "winhttp.lib")

// ---------------------------------------------------------------------
// Config (loaded from watchdog_config.json next to the exe; falls back
// to sane defaults if the file is missing so the tool still runs)
// ---------------------------------------------------------------------
struct Config {
    std::wstring targetProcessName = L"SHARK-AntiCheat.exe";
    int pollIntervalMs = 1000;
    std::string discordWebhook;          // empty = log-file only, no Discord
    std::wstring logFilePath = L"watchdog.log";
    std::string hostname;
    bool alertOnNewThreads = false;      // usually noisy/normal; off by default
};

static Config g_config;
static std::mutex g_logMutex;

// ---------------------------------------------------------------------
// Logging (always-on, since there's no console to see anything in)
// ---------------------------------------------------------------------
static std::string nowTimestamp() {
    std::time_t t = std::time(nullptr);
    std::tm tmv{};
    localtime_s(&tmv, &t);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmv);
    return std::string(buf);
}

static void logLine(const std::string& level, const std::string& msg) {
    std::lock_guard<std::mutex> lock(g_logMutex);
    std::filesystem::path logFsPath(g_config.logFilePath);
    std::ofstream f(logFsPath, std::ios::app);
    if (f.good()) {
        f << nowTimestamp() << " [" << level << "] " << msg << "\n";
    }
}

static void logInfo(const std::string& msg) { logLine("INFO", msg); }
static void logWarn(const std::string& msg) { logLine("WARN", msg); }
static void logAlert(const std::string& msg) { logLine("ALERT", msg); }

// ---------------------------------------------------------------------
// Discord webhook (fire-and-forget on a detached thread so a slow/hung
// network call never stalls the monitoring loop)
// ---------------------------------------------------------------------
static std::string getHostnameUtf8() {
    char buf[256];
    DWORD size = sizeof(buf);
    if (GetComputerNameA(buf, &size)) return std::string(buf);
    return "unknown-host";
}

static bool crackUrl(const std::string& url, std::wstring& host, std::wstring& path,
                      INTERNET_PORT& port, bool& https) {
    std::wstring wurl(url.begin(), url.end());
    URL_COMPONENTS uc{};
    uc.dwStructSize = sizeof(uc);
    wchar_t hostBuf[256]{}, pathBuf[2048]{};
    uc.lpszHostName = hostBuf; uc.dwHostNameLength = 256;
    uc.lpszUrlPath = pathBuf; uc.dwUrlPathLength = 2048;
    if (!WinHttpCrackUrl(wurl.c_str(), (DWORD)wurl.size(), 0, &uc)) return false;
    host = hostBuf;
    path = pathBuf;
    port = uc.nPort;
    https = (uc.nScheme == INTERNET_SCHEME_HTTPS);
    return true;
}

static void postDiscordEmbedSync(const std::string& webhookUrl, const std::string& title,
                                  const std::string& description, unsigned color) {
    std::wstring host, path; INTERNET_PORT port; bool https;
    if (!crackUrl(webhookUrl, host, path, port, https)) return;

    nlohmann::json embed;
    embed["title"] = std::string("\xF0\x9F\x9A\xA8 ") + title;
    embed["description"] = description;
    embed["color"] = color;
    embed["footer"] = {{"text", "SHARK-AntiCheat Watchdog @ " + getHostnameUtf8()}};

    nlohmann::json payload;
    payload["embeds"] = nlohmann::json::array({embed});
    payload["username"] = "AC Watchdog";
    std::string body = payload.dump();

    HINTERNET hSession = WinHttpOpen(L"ACWatchdog/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
                                      WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return;
    // Keep this fast: a hung webhook shouldn't block us for long.
    WinHttpSetTimeouts(hSession, 3000, 3000, 5000, 5000);

    HINTERNET hConnect = WinHttpConnect(hSession, host.c_str(), port, 0);
    if (hConnect) {
        DWORD flags = https ? WINHTTP_FLAG_SECURE : 0;
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", path.c_str(), nullptr,
                                                 WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, flags);
        if (hRequest) {
            std::wstring headers = L"Content-Type: application/json\r\n";
            WinHttpSendRequest(hRequest, headers.c_str(), (DWORD)-1,
                                (LPVOID)body.data(), (DWORD)body.size(), (DWORD)body.size(), 0);
            WinHttpReceiveResponse(hRequest, nullptr);
            WinHttpCloseHandle(hRequest);
        }
        WinHttpCloseHandle(hConnect);
    }
    WinHttpCloseHandle(hSession);
}

static void raiseAlert(const std::string& title, const std::string& description, unsigned color = 0xE74C3C) {
    logAlert(title + " -- " + description);
    if (!g_config.discordWebhook.empty()) {
        std::thread(postDiscordEmbedSync, g_config.discordWebhook, title, description, color).detach();
    }
}

// ---------------------------------------------------------------------
// Config loading
// ---------------------------------------------------------------------
static std::wstring utf8ToWide(const std::string& s) {
    if (s.empty()) return L"";
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring w(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], len);
    if (!w.empty() && w.back() == L'\0') w.pop_back();
    return w;
}

static std::string wideToUtf8(const std::wstring& w) {
    if (w.empty()) return "";
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &s[0], len, nullptr, nullptr);
    if (!s.empty() && s.back() == '\0') s.pop_back();
    return s;
}

static void loadConfig(const std::wstring& exeDir) {
    std::wstring cfgPath = exeDir + L"\\watchdog_config.json";
    std::filesystem::path cfgFsPath(cfgPath);
    std::ifstream f(cfgFsPath);
    if (!f.good()) {
        logWarn("watchdog_config.json not found next to the exe; using built-in defaults "
                "(target=SHARK-AntiCheat.exe, poll=1000ms, no Discord webhook).");
        return;
    }
    try {
        nlohmann::json j;
        f >> j;
        if (j.contains("target_process_name"))
            g_config.targetProcessName = utf8ToWide(j["target_process_name"].get<std::string>());
        if (j.contains("poll_interval_ms"))
            g_config.pollIntervalMs = j["poll_interval_ms"].get<int>();
        if (j.contains("discord_webhook")) {
            std::string wh = j["discord_webhook"].get<std::string>();
            if (!wh.empty() && wh.find("YOUR_WEBHOOK") == std::string::npos)
                g_config.discordWebhook = wh;
        }
        if (j.contains("log_file_path"))
            g_config.logFilePath = utf8ToWide(j["log_file_path"].get<std::string>());
        if (j.contains("alert_on_new_threads"))
            g_config.alertOnNewThreads = j["alert_on_new_threads"].get<bool>();
    } catch (const std::exception& e) {
        logWarn(std::string("Failed to parse watchdog_config.json, using defaults: ") + e.what());
    }
}

// ---------------------------------------------------------------------
// Process/thread snapshotting
// ---------------------------------------------------------------------
static DWORD findPidByName(const std::wstring& name) {
    DWORD pid = 0;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return 0;
    PROCESSENTRY32W entry{};
    entry.dwSize = sizeof(entry);
    if (Process32FirstW(snap, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, name.c_str()) == 0) {
                pid = entry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snap, &entry));
    }
    CloseHandle(snap);
    return pid;
}

// NT native structures for per-thread state (State/WaitReason let us see
// "this thread is suspended" without opening the thread at all -- same
// technique Process Hacker/System Informer use).
struct SYSTEM_THREAD_INFORMATION_MIN {
    LONGLONG KernelTime;
    LONGLONG UserTime;
    LONGLONG CreateTime;
    ULONG WaitTime;
    ULONG _pad;
    PVOID StartAddress;
    PVOID ClientIdUniqueProcess;
    PVOID ClientIdUniqueThread;
    LONG Priority;
    LONG BasePriority;
    ULONG ContextSwitches;
    ULONG ThreadState;   // 5 = Waiting
    ULONG WaitReason;    // 5 = Suspended
};

struct SYSTEM_PROCESS_INFORMATION_MIN {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    BYTE Reserved1[48];
    USHORT ImageName_Length;
    USHORT ImageName_MaxLength;
    PVOID ImageName_Buffer;
    LONG BasePriority;
    PVOID UniqueProcessId;
    PVOID Reserved2;
    ULONG HandleCount;
    ULONG SessionId;
    PVOID Reserved3;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG Reserved4;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    PVOID Reserved5;
    SIZE_T QuotaPagedPoolUsage;
    PVOID Reserved6;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    // NOTE: six LARGE_INTEGER I/O counters sit here, immediately before
    // the Threads[] array -- ReadOperationCount, WriteOperationCount,
    // OtherOperationCount, ReadTransferCount, WriteTransferCount,
    // OtherTransferCount. Missing these shifts every thread-array read
    // 48 bytes into the wrong memory, which is exactly what was causing
    // the false "thread terminated" alerts (we were reading a byte
    // transfer counter and mistaking it for a shifting thread ID).
    LONGLONG ReadOperationCount;
    LONGLONG WriteOperationCount;
    LONGLONG OtherOperationCount;
    LONGLONG ReadTransferCount;
    LONGLONG WriteTransferCount;
    LONGLONG OtherTransferCount;
};

// Compile-time sanity check: this struct's tail (through the six I/O
// counters) must land on an 8-byte boundary so the Threads[] array that
// immediately follows in the real NT struct starts correctly aligned.
// If this ever fails after an edit, the thread-array offset math below
// will silently read garbage again (this is exactly what the missing
// I/O counters bug looked like).
static_assert(sizeof(SYSTEM_PROCESS_INFORMATION_MIN) % 8 == 0,
              "SYSTEM_PROCESS_INFORMATION_MIN size must be 8-byte aligned");

typedef LONG(WINAPI* NtQuerySystemInformation_t)(ULONG, PVOID, ULONG, PULONG);
static NtQuerySystemInformation_t NtQuerySystemInformation =
    reinterpret_cast<NtQuerySystemInformation_t>(
        reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"ntdll.dll"), "NtQuerySystemInformation")));

constexpr ULONG SystemProcessInformation = 5;
constexpr ULONG STATUS_INFO_LENGTH_MISMATCH = 0xC0000004;
constexpr ULONG THREAD_STATE_WAITING = 5;
constexpr ULONG WAIT_REASON_SUSPENDED = 5;

struct ThreadState {
    bool suspended = false;
    ULONG contextSwitches = 0;
};

// Returns false if the target process wasn't found at all in the system
// snapshot (i.e. it has exited); otherwise fills `out` with tid -> state.
static bool snapshotThreadStates(DWORD targetPid, std::unordered_map<DWORD, ThreadState>& out) {
    out.clear();
    if (!NtQuerySystemInformation) return false;

    ULONG size = 1 << 20;
    std::vector<BYTE> buf;
    for (;;) {
        buf.resize(size);
        ULONG returned = 0;
        LONG status = NtQuerySystemInformation(SystemProcessInformation, buf.data(), size, &returned);
        if (status == 0) break;
        if ((ULONG)status == STATUS_INFO_LENGTH_MISMATCH) {
            size = std::max(size * 2, returned + (1u << 16));
            if (size > (1u << 27)) return false;
            continue;
        }
        return false;
    }

    BYTE* base = buf.data();
    ULONG offset = 0;
    bool foundProcess = false;

    for (;;) {
        auto* proc = reinterpret_cast<SYSTEM_PROCESS_INFORMATION_MIN*>(base + offset);
        DWORD pid = static_cast<DWORD>(reinterpret_cast<uintptr_t>(proc->UniqueProcessId));

        if (pid == targetPid) {
            foundProcess = true;
            auto* threads = reinterpret_cast<SYSTEM_THREAD_INFORMATION_MIN*>(
                base + offset + sizeof(SYSTEM_PROCESS_INFORMATION_MIN));
            for (ULONG i = 0; i < proc->NumberOfThreads; ++i) {
                DWORD tid = static_cast<DWORD>(reinterpret_cast<uintptr_t>(threads[i].ClientIdUniqueThread));
                ThreadState ts;
                ts.suspended = (threads[i].ThreadState == THREAD_STATE_WAITING &&
                                 threads[i].WaitReason == WAIT_REASON_SUSPENDED);
                ts.contextSwitches = threads[i].ContextSwitches;
                out[tid] = ts;
            }
            break;
        }

        if (proc->NextEntryOffset == 0) break;
        offset += proc->NextEntryOffset;
    }

    return foundProcess;
}

// ---------------------------------------------------------------------
// Monitoring loop
// ---------------------------------------------------------------------
static void monitorLoop() {
    bool wasRunning = false;
    bool wholeProcessSuspendedAlerted = false;
    DWORD lastPid = 0;
    std::unordered_map<DWORD, ThreadState> prevThreads;

    logInfo("Watchdog started. Target process: " + wideToUtf8(g_config.targetProcessName) +
            " | poll_interval_ms=" + std::to_string(g_config.pollIntervalMs));

    for (;;) {
        DWORD pid = findPidByName(g_config.targetProcessName);

        if (pid == 0) {
            if (wasRunning) {
                raiseAlert("Target process terminated",
                           "SHARK-AntiCheat.exe (pid " + std::to_string(lastPid) +
                           ") is no longer running. It either exited normally or was killed.",
                           0xE74C3C);
                wasRunning = false;
                prevThreads.clear();
                lastPid = 0;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(g_config.pollIntervalMs));
            continue;
        }

        std::unordered_map<DWORD, ThreadState> currentThreads;
        bool foundInSnapshot = snapshotThreadStates(pid, currentThreads);

        if (!wasRunning || pid != lastPid) {
            logInfo("Target process detected, pid=" + std::to_string(pid) +
                    ", thread_count=" + std::to_string(currentThreads.size()));
            wasRunning = true;
            lastPid = pid;
            prevThreads = currentThreads;
            std::this_thread::sleep_for(std::chrono::milliseconds(g_config.pollIntervalMs));
            continue;
        }

        if (!foundInSnapshot) {
            // toolhelp still sees the PID (rare race) but the NT query
            // snapshot doesn't -- treat conservatively as "can't verify",
            // log and move on rather than false-alarming.
            logWarn("Could not read thread states for pid=" + std::to_string(pid) + " this cycle.");
            std::this_thread::sleep_for(std::chrono::milliseconds(g_config.pollIntervalMs));
            continue;
        }

        // Diff against previous snapshot.
        size_t killedThreads = 0, newlySuspended = 0, resumed = 0, newThreads = 0;
        size_t totalNow = currentThreads.size();
        size_t suspendedNow = 0;

        for (auto& [tid, state] : currentThreads) {
            if (state.suspended) suspendedNow++;

            auto it = prevThreads.find(tid);
            if (it == prevThreads.end()) {
                newThreads++;
                if (g_config.alertOnNewThreads) {
                    logInfo("New thread observed in target process: tid=" + std::to_string(tid));
                }
                continue;
            }
            if (state.suspended && !it->second.suspended) {
                newlySuspended++;
                raiseAlert("Target process thread suspended",
                           "Thread " + std::to_string(tid) + " in SHARK-AntiCheat.exe (pid " +
                           std::to_string(pid) + ") was just suspended. This is a common technique "
                           "for disabling anti-cheat without triggering a process-exit event.",
                           0xE67E22);
            } else if (!state.suspended && it->second.suspended) {
                resumed++;
                logInfo("Thread resumed: tid=" + std::to_string(tid));
            }
        }

        for (auto& [tid, prevState] : prevThreads) {
            if (currentThreads.find(tid) == currentThreads.end()) {
                killedThreads++;
                raiseAlert("Target process thread terminated",
                           "Thread " + std::to_string(tid) + " in SHARK-AntiCheat.exe (pid " +
                           std::to_string(pid) + ") disappeared without the process itself exiting "
                           "-- possible selective thread-kill attack.",
                           0xE67E22);
            }
        }

        // Whole-process suspension: every thread suspended at once, and
        // the thread count didn't just collapse to zero (that's covered
        // by the "process terminated" branch above on the next cycle).
        if (totalNow > 0 && suspendedNow == totalNow && killedThreads == 0) {
            if (!wholeProcessSuspendedAlerted) {
                raiseAlert("Target process appears fully suspended",
                           "All " + std::to_string(totalNow) + " threads of SHARK-AntiCheat.exe (pid " +
                           std::to_string(pid) + ") are currently suspended. The process is still "
                           "resident but not executing -- consistent with an NtSuspendProcess-style "
                           "freeze attack rather than a normal exit.",
                           0xE74C3C);
                wholeProcessSuspendedAlerted = true;
            }
        } else {
            wholeProcessSuspendedAlerted = false; // allow a future full-suspend event to alert again
        }

        prevThreads = std::move(currentThreads);
        std::this_thread::sleep_for(std::chrono::milliseconds(g_config.pollIntervalMs));
    }
}

// ---------------------------------------------------------------------
// Entry point (GUI subsystem -- no console window is ever created)
// ---------------------------------------------------------------------
int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int) {
    // Single-instance guard so you don't end up with duplicate alert
    // spam if the watchdog gets launched twice.
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, L"Global\\SharkAntiCheatWatchdog_SingleInstance");
    if (hMutex && GetLastError() == ERROR_ALREADY_EXISTS) {
        return 0; // already running; exit quietly
    }

    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    auto pos = exeDir.find_last_of(L"\\/");
    exeDir = (pos == std::wstring::npos) ? L"." : exeDir.substr(0, pos);

    // Resolve log path relative to the exe directory if a relative path
    // was given, so it doesn't depend on the process's working directory.
    loadConfig(exeDir);
    if (g_config.logFilePath.find(L":") == std::wstring::npos &&
        g_config.logFilePath.substr(0, 2) != L"\\\\") {
        g_config.logFilePath = exeDir + L"\\" + g_config.logFilePath;
    }

    logInfo("=== SHARK-AntiCheat Watchdog starting ===");
    monitorLoop(); // never returns

    if (hMutex) CloseHandle(hMutex);
    return 0;
}
