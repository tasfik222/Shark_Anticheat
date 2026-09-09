@echo off
echo ================================
echo  Building Red Eye AntiCheat...
echo ================================
echo.

g++ -o "Red Eye - AntiCheat for HD-Player.exe" "Red_Eye_-_AntiCheat_for_HD-Player.cpp" ^
-lpsapi -lwbemuuid -lwininet -lshell32 -ladvapi32 -lwintrust -lcrypt32 ^
-lole32 -loleaut32 -luuid -lws2_32 ^
-std=c++17 -O2

if %errorlevel%==0 (
    echo.
    echo [SUCCESS] Red Eye - AntiCheat for HD-Player.exe ready!
) else (
    echo.
    echo [FAILED] Error occurred. Check above messages.
)
echo.
pause
