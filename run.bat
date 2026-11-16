@echo off
rem ==========================================================================
rem  GLFrame - run script
rem  Opens the interactive Sierpinski gasket window.
rem  Does NOT build - use build.bat for that (build and run are separate).
rem ==========================================================================

setlocal
set "ROOT=%~dp0"
set "EXE="

if exist "%ROOT%x64\Release\GLFrame.exe" set "EXE=%ROOT%x64\Release\GLFrame.exe"
if not defined EXE if exist "%ROOT%x64\Debug\GLFrame.exe" set "EXE=%ROOT%x64\Debug\GLFrame.exe"
if not defined EXE goto missing

rem Make sure a leftover GLFRAME_CAPTURE cannot turn the run into a
rem headless screenshot - see Framebuffer.h.
set "GLFRAME_CAPTURE="
start "GLFrame" "%EXE%"
goto end

:missing
echo [ERROR] GLFrame.exe not found.
echo         Run build.bat first.
echo.
pause

:end
endlocal
