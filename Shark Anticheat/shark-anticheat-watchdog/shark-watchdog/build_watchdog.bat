@echo off
echo ================================
echo  Building SHARK-AntiCheat Watchdog...
echo ================================
echo.

g++ -o "SHARK-Watchdog.exe" "src\main.cpp" ^
-lwinhttp ^
-std=c++17 -O2 -municode -mwindows

if %errorlevel%==0 (
    echo.
    echo [SUCCESS] SHARK-Watchdog.exe ready!
    echo Copy watchdog_config.json next to the exe before running.
) else (
    echo.
    echo [FAILED] Error occurred. Check above messages.
)
echo.
pause
