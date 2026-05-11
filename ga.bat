@echo off
REM gitauto alias script - allows using 'ga' instead of 'gitauto'

REM Get the directory where this script is located
set "SCRIPT_DIR=%~dp0"

REM Call gitauto.exe with all passed arguments
"%SCRIPT_DIR%src\gitauto.exe" %*