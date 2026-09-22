@echo off
setlocal
cd /d "%~dp0"
set "PATH=%~dp0;%PATH%"
set "XDG_DATA_DIRS=%~dp0share"
set "GSETTINGS_SCHEMA_DIR=%~dp0share\glib-2.0\schemas"
set "ARBORETUM_DIAGNOSTIC=1"
set "LOG=%TEMP%\Arboretum-diagnose.txt"
if /i "%~1"=="cairo" set "GSK_RENDERER=cairo"
echo Arboretum Windows startup diagnostic > "%LOG%"
echo Renderer: %GSK_RENDERER% >> "%LOG%"
"%~dp0arboretum.exe" >> "%LOG%" 2>&1
set "RESULT=%ERRORLEVEL%"
echo Exitcode: %RESULT% >> "%LOG%"
type "%LOG%"
echo.
echo Log: %LOG%
pause
