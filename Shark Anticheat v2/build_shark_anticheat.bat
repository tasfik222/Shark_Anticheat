@echo off
echo ================================
echo  Building SHARK AntiCheat...
echo ================================
echo.

g++ -o "SHARK-AntiCheat.exe" "SHARK_AntiCheat.cpp" ^
-lpsapi -lwbemuuid -lwininet -lshell32 -ladvapi32 -lwintrust -lcrypt32 ^
-lole32 -loleaut32 -luuid -lws2_32 ^
-std=c++17 -O2

if %errorlevel%==0 (
    echo.
    echo [SUCCESS] SHARK-AntiCheat.exe ready!
) else (
    echo.
    echo [FAILED] Error occurred. Check above messages.
)
echo.
pause
