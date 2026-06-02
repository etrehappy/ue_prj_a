@echo off
set CHAR_SERVER_DIR=D:\EpicGames\UE_Projects\ue_prj_a\CharacterServer
set PYTHON=python
set NAME="Project_A Character Server"

echo === Starting Project_A Character Server ===
echo URL: http://127.0.0.1:7784
echo Press Ctrl+C to stop.
echo.

REM start %NAME% %PYTHON% "%CHAR_SERVER_DIR%\character_server.py"
start %NAME% "%CHAR_SERVER_DIR%\character_server.exe"

REM pause
REM netstat -ano | findstr :7784