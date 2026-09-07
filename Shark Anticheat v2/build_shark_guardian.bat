@echo off
echo ================================
echo  Building SHARK Guardian...
echo ================================
echo.

g++ -o "SHARKGuardian.exe" "SHARK_Guardian.cpp" ^
-lpsapi -lwininet -lshell32 -ladvapi32 ^
-std=c++17 -O2

if %errorlevel%==0 (
    echo.
    echo [SUCCESS] SHARKGuardian.exe ready!
) else (
    echo.
    echo [FAILED] Error occurred. Check above messages.
)
echo.
pause
