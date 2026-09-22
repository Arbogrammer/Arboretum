@echo off
setlocal
set "ARBORETUM_IO_SMOKE_TEST=1"
call "%~dp0Diagnose.bat" %*
