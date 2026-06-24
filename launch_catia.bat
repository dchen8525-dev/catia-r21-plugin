@echo off
setlocal

set "CATSTART=C:\Program Files (x86)\Dassault Systemes\B21\intel_a\code\bin\CATSTART.exe"
set "CATENV=C:\ProgramData\DassaultSystemes\CATEnv"
set "LOGDIR=%LOCALAPPDATA%\DassaultSystemes\CATTemp"
set "LAUNCHLOG=%LOGDIR%\HelloWorldLauncher.log"

if not exist "%LOGDIR%" mkdir "%LOGDIR%"
>>"%LAUNCHLOG%" echo ============================================================
>>"%LAUNCHLOG%" echo [%date% %time%] launch_catia.bat entered
>>"%LAUNCHLOG%" echo CATSTART=%CATSTART%
>>"%LAUNCHLOG%" echo CATENV=%CATENV%

if not exist "%CATSTART%" (
  >>"%LAUNCHLOG%" echo ERROR: CATSTART missing
  echo ERROR: CATIA V5R21 CATSTART.exe was not found.
  exit /b 2
)

>>"%LAUNCHLOG%" echo Starting CATSTART with CATIA_P3.V5R21.B21
"%CATSTART%" -env "CATIA_P3.V5R21.B21" -direnv "%CATENV%" -run "cmd.exe /c call %~dp0_launch_runtime.bat"
set "RC=%errorlevel%"
>>"%LAUNCHLOG%" echo [%date% %time%] CATSTART returned rc=%RC%
exit /b %RC%
