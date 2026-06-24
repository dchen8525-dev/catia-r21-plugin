@echo off
setlocal
set "CATSTART=C:\Program Files (x86)\Dassault Systemes\B21\intel_a\code\bin\CATSTART.exe"
set "CATENV=C:\ProgramData\DassaultSystemes\CATEnv"

if not exist "%CATSTART%" (
  echo ERROR: CATIA V5R21 CAA RADE CATSTART.exe was not found.
  exit /b 2
)

del /Q "%~dp0.build-exit-code" 2>nul
"%CATSTART%" -env "CAA_RADE.V5R21_RADE21.B21" -direnv "%CATENV%" -run "cmd.exe /c call %~dp0_build_rade.bat"
if errorlevel 1 exit /b %errorlevel%

:wait
if not exist "%~dp0.build-exit-code" (
  >nul ping 127.0.0.1 -n 2
  goto wait
)
set /p BUILD_RC=<"%~dp0.build-exit-code"
del /Q "%~dp0.build-exit-code"
if not "%BUILD_RC%"=="0" (
  echo ERROR: RADE build failed. See rade_build.log and rade_preq.log.
  exit /b %BUILD_RC%
)
echo RADE x64 build and runtime view completed successfully.
exit /b 0
