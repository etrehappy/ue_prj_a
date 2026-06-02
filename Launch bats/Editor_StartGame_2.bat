@echo off
set UE_EDITOR="D:\EpicGames\UE_git_5_7\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set PROJECT_PATH="D:\EpicGames\UE_Projects\ue_prj_a\Project_A\Project_A.uproject"
set CLIENT_NAME="Project_A Client Second"
@REM set SERVER_PORT=7777

set NO_PROXY=127.0.0.1,localhost
set no_proxy=127.0.0.1,localhost
set HTTP_PROXY=
set HTTPS_PROXY=
set http_proxy=
set https_proxy=

REM === Запуск редактора с параметрами ===

start %CLIENT_NAME% %UE_EDITOR% ^
    %PROJECT_PATH% -nosteam -game -log -WINDOWED -ResX=800 -ResY=450 ^
     -abslog="%~dp0Log\LogClientSecond.txt"

@REM start "Project_A Client" "D:\EpicGames\UE_git_5_7\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe" ^
@REM "D:\EpicGames\UE_Projects\ue_prj_a\Project_A\Project_A.uproject" -nosteam -game -log -WINDOWED -ResX=800 -ResY=450 

@REM -steam 127.0.0.1:7777 -abslog="%~dp0LogClientEditor.txt"