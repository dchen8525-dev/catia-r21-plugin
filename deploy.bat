@echo off
setlocal
set "ACTION=%~1"
if not defined ACTION set "ACTION=Install"

fltmc.exe >nul 2>&1
if errorlevel 1 (
  powershell.exe -NoProfile -Command ^
    "Start-Process -FilePath '%~f0' -ArgumentList '%ACTION%' -Verb RunAs -Wait"
  exit /b %errorlevel%
)

powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0deploy.ps1" -Action "%ACTION%"
exit /b %errorlevel%
