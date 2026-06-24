@echo off
setlocal
cd /d "%~dp0"
set "RESULT=%~dp0.build-exit-code"
set "RC=0"

set "MkmkINSTALL_PATH=C:\PROGRA~2\DASSAU~1\B21"
call "%MkmkINSTALL_PATH%\intel_a\code\command\MkmkSetenv.bat" > "%~dp0rade_profile.log" 2>&1
if errorlevel 1 set "RC=%errorlevel%"
if not "%RC%"=="0" goto done

set "RADECATSettingPath=%APPDATA%\DassaultSystemes\CATSettings\RADE"
set "MkmkROOT_PATH=%MkmkROOT_PATH%;C:\PROGRA~1\DASSAU~1\B21"

call mkGetPreq -p C:\PROGRA~1\DASSAU~1\B21 > "%~dp0rade_preq.log" 2>&1
if errorlevel 1 set "RC=%errorlevel%"
if not "%RC%"=="0" goto done

call mkmk -a > "%~dp0rade_build.log" 2>&1
if errorlevel 1 set "RC=%errorlevel%"
if not "%RC%"=="0" goto done

call mkCreateRuntimeView > "%~dp0rade_runtime.log" 2>&1
if errorlevel 1 set "RC=%errorlevel%"

:done
> "%RESULT%" echo %RC%
exit /b %RC%
