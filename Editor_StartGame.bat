@echo off
set UE_EDITOR="\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set PROJECT_PATH="%~dp0Project_A\Project_A.uproject"
set CLIENT_NAME="Project_A Client"

if not exist %UE_EDITOR% (
    echo ERROR: UnrealEditor.exe not found at: %UE_EDITOR%
    echo Please update the UE_EDITOR path in this script or open the README
    pause
    exit /b
)

REM ---------- Client ----------
echo Start Client...
start %CLIENT_NAME% %UE_EDITOR% ^
    %PROJECT_PATH% -nosteam -game -WINDOWED -ResX=800 -ResY=450 ^
        -abslog="%~dp0Logs\LogClient.txt"
