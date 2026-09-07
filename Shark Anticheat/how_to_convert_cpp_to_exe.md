# SHARK AntiCheat & Guardian — Build Instructions

A complete guide to compiling `SHARK_AntiCheat.cpp` and `SHARK_Guardian.cpp` on Windows using MinGW (g++).

## 1. Download MinGW

Grab the latest build from the release page below:

👉 [niXman/mingw-builds-binaries — Latest Release](https://github.com/niXman/mingw-builds-binaries/releases/latest)

Specific file:

```
x86_64-16.1.0-release-win32-seh-msvcrt-rt_v14-rev1.7z
```

## 2. Extract the Archive

Extract the downloaded `.7z` file to:

```
C:\mingw64
```

> ⚠️ After extracting, make sure `g++.exe` is located inside `C:\mingw64\bin`. Some archives extract into an extra nested `mingw64` subfolder — if that happens, move the inner contents up to `C:\mingw64`.

## 3. Add MinGW to System PATH

Open **Command Prompt as Administrator** and run:

```cmd
setx PATH "%PATH%;C:\mingw64\bin" /M
```

Then **open a new Command Prompt / terminal window** (the old one won't reflect the updated PATH) and verify:

```cmd
g++ --version
```

If installed correctly, this will print the g++ version info.

## 4. Compile

Navigate to your project folder and run the following commands:

```cmd
g++ SHARK_AntiCheat.cpp -o SHARK_AntiCheat.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
g++ SHARK_Guardian.cpp -o SHARK_Guardian.exe -lole32 -loleaut32 -lwininet -lwintrust -lwbemuuid
```

Once compiled successfully, `SHARK_AntiCheat.exe` and `SHARK_Guardian.exe` will be created in the same folder.

### Linked Libraries

| Library | Purpose |
|---|---|
| `ole32` | COM (Component Object Model) support |
| `oleaut32` | OLE Automation functions |
| `wininet` | Internet/HTTP request handling |
| `wintrust` | File signature / Authenticode verification |
| `wbemuuid` | WMI (Windows Management Instrumentation) queries |

## Troubleshooting

- **`'g++' is not recognized`** — Restart your terminal after the PATH update; reboot your PC if it still doesn't work.
- **`undefined reference` errors** — Double-check the library flag order and that the `.cpp` filenames match exactly.
- **`.7z` won't extract** — Install [7-Zip](https://www.7-zip.org/) first.

---

**Required tools:** [MinGW-w64 (niXman build)](https://github.com/niXman/mingw-builds-binaries) · [7-Zip](https://www.7-zip.org/)
