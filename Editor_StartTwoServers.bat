@echo off
set UE_EDITOR="\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set PROJECT_PATH="%~dp0Project_A\Project_A.uproject"

if not exist %UE_EDITOR% (
    echo ERROR: UnrealEditor.exe not found at: %UE_EDITOR%
    echo Please update the UE_EDITOR path in this script or open the README
    pause
    exit /b
)

REM ---------- Ferst Server ----------
set MAP_NAME=/Game/Project_A_Root/Levels/StartMap/ServerHubMap
set SERVER_NAME="Project_A HubServer"
set SERVER_PORT=7777

echo Start server HUB...
start %SERVER_NAME% %UE_EDITOR% %PROJECT_PATH% ^
     %MAP_NAME% -server -log -nosteam -port=%SERVER_PORT% ^
        -abslog="%~dp0Logs\LogHubServer.txt"

REM ---------- Second Server ----------
set MAP_NAME=/Game/ThirdPerson/Lvl_ThirdPerson
set SERVER_NAME="Project_A WorldServer"
set SERVER_PORT=7778

echo Start server MainWorld...
start %SERVER_NAME% %UE_EDITOR% %PROJECT_PATH% ^
     %MAP_NAME% -server -log -nosteam -port=%SERVER_PORT% ^
        -abslog="%~dp0Logs\LogWorldServer.txt"

