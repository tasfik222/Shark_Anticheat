@echo off
echo ================================
echo  Building Red Eye Guardian...
echo ================================
echo.

g++ -o "RedEyeGuardian.exe" "RedEye_Guardian.cpp" ^
-lpsapi -lwininet -lshell32 -ladvapi32 ^
-std=c++17 -O2

if %errorlevel%==0 (
    echo.
    echo [SUCCESS] RedEyeGuardian.exe ready!
) else (
    echo.
    echo [FAILED] Error occurred. Check above messages.
)
echo.
pause
