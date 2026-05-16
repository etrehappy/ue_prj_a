@echo off
set UE_EDITOR="D:\EpicGames\UE_git_5_7\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set PROJECT_PATH="D:\EpicGames\UE_Projects\ue_prj_a\Project_A\Project_A.uproject"
set SERVER_NAME="Project_A LobbyServer"
set MAP_NAME=/Game/Project_A_Root/Levels/Lobby/LobbyMap
set SERVER_PORT=7781

REM === Запуск редактора с параметрами ===
start %SERVER_NAME% %UE_EDITOR% %PROJECT_PATH% ^
     %MAP_NAME% -server -nosteam -log -port=%SERVER_PORT%


@REM start "Project_A HubServer" "D:\EpicGames\UE_git_5_7\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe" ^
@REM "D:\EpicGames\UE_Projects\ue_prj_a\Project_A\Project_A.uproject" -server -nosteam -log -port=7777 

@REM -nosteam -steam -force_steamclient_link
