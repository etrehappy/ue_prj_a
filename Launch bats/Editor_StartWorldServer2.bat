@echo off
set UE_EDITOR="D:\EpicGames\UE_git_5_7\UnrealEngine\Engine\Binaries\Win64\UnrealEditor.exe"
set PROJECT_PATH="D:\EpicGames\UE_Projects\ue_prj_a\Project_A\Project_A.uproject"
set SERVER_NAME="Project_A Server Asteros"
REM set MAP_NAME=/Game/ThirdPerson/Lvl_ThirdPerson
set MAP_NAME=/Game/Project_A_Root/Levels/Map
set SERVER_PORT=7779

REM === Запуск редактора с параметрами ===
start %SERVER_NAME% %UE_EDITOR% %PROJECT_PATH% ^
     %MAP_NAME% -server -nosteam -log ^
     -ServerId=MainWorld02 -ServerName=Asteros -PublicAddress=127.0.0.1:7779 ^
     -port=%SERVER_PORT% ^
     -abslog="%~dp0Log\LogWorldServerAsteros.txt"


@REM -nosteam -steam -force_steamclient_link