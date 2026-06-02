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

set NO_PROXY=127.0.0.1,localhost
set no_proxy=127.0.0.1,localhost
set HTTP_PROXY=
set HTTPS_PROXY=
set http_proxy=
set https_proxy=

REM ---------- Client ----------
echo Start Client...
start %CLIENT_NAME% %UE_EDITOR% ^
    %PROJECT_PATH% -nosteam -game -WINDOWED -ResX=800 -ResY=450 ^
        -abslog="%~dp0Logs\LogClient.txt"
