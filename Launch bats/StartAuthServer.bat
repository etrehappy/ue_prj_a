@echo off
set AUTH_SERVER_DIR=D:\EpicGames\UE_Projects\ue_prj_a\AuthServer
set PYTHON=python
set NAME="Project_A Auth Server"

echo === Starting Project_A Auth Server ===
echo URL: http://127.0.0.1:7783
echo Press Ctrl+C to stop.
echo.

REM start %NAME% %PYTHON% "%AUTH_SERVER_DIR%\auth_server.py"
start %NAME% "%AUTH_SERVER_DIR%\auth_server.exe"

REM pause
REM netstat -ano | findstr :7783