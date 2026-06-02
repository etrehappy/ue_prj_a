@echo off
set AUTH_SERVER_DIR=.\AuthServer
set CHAR_SERVER_DIR=.\CharacterServer
set PYTHON=python
set NAME_AUTH="Project_A Auth Server"
set NAME_Character="Project_A Character Server"

REM ///////////////////////////////////////////////////////////
echo === Starting Project_A Auth Server ===
echo URL: http://127.0.0.1:7783
echo Press Ctrl+C to stop.
echo.

start %NAME_AUTH% "%AUTH_SERVER_DIR%\auth_server.exe"

REM ///////////////////////////////////////////////////////////
echo === Starting Project_A Character Server ===
echo URL: http://127.0.0.1:7784
echo Press Ctrl+C to stop.
echo.

start %NAME_Character% "%CHAR_SERVER_DIR%\character_server.exe"

REM netstat -ano | findstr :7783