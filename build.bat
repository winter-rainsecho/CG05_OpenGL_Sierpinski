@echo off
rem ==========================================================================
rem  GLFrame - build script
rem
rem  Compiles GLFrame (MFC + OpenGL) without the Visual Studio IDE.
rem  The MSVC toolset and the Windows SDK are located automatically, so this
rem  script does NOT depend on vcvarsall.bat (which calls reg.exe and can be
rem  blocked by security software).
rem
rem  Usage: build.bat [Debug|Release] [nopause]
rem
rem  This script only BUILDS and runs a headless self-check screenshot.
rem  It does NOT open the interactive window - use run.bat for that.
rem
rem  Keep this file pure ASCII with CRLF line endings: cmd.exe parses batch
rem  files with the OEM code page, and every path below contains "Program
rem  Files (x86)", whose parentheses break cmd's block parser. Hence the
rem  goto/label style instead of parenthesised IF blocks.
rem ==========================================================================

setlocal

set "CFG=%~1"
if "%CFG%"=="" set "CFG=Release"

set "ROOT=%~dp0"
set "PROJ=%ROOT%GLFrame"
set "OUT=%ROOT%x64\%CFG%"
set "OBJ=%OUT%\obj"
set "STATUS=%ROOT%build.status"

rem A local C++ build needs no proxy, and a duplicate HTTP_PROXY /
rem http_proxy pair makes MSBuild-based tooling throw. Drop them.
set "http_proxy="
set "https_proxy="
set "all_proxy="
set "no_proxy="

rem ---------------- locate Visual Studio Build Tools / IDE ----------------
set "VSROOT="
if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC" set "VSROOT=C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools"
if not defined VSROOT if exist "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Tools\MSVC" set "VSROOT=C:\Program Files\Microsoft Visual Studio\2022\BuildTools"
if not defined VSROOT if exist "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC" set "VSROOT=C:\Program Files\Microsoft Visual Studio\2022\Community"
if not defined VSROOT if exist "C:\Program Files (x86)\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC" set "VSROOT=C:\Program Files (x86)\Microsoft Visual Studio\2022\Community"
if not defined VSROOT goto err_vs

rem Newest toolset = first entry when sorted by name descending (14.x names
rem compare correctly as strings, e.g. 14.44.35207 > 14.38.33130).
set "MSVCVER="
for /f "usebackq delims=" %%D in (`dir /b /ad /o-n "%VSROOT%\VC\Tools\MSVC" 2^>nul`) do if not defined MSVCVER set "MSVCVER=%%D"
if not defined MSVCVER goto err_vs

rem ---------------- locate the Windows 10/11 SDK ----------------
set "SDKROOT=C:\Program Files (x86)\Windows Kits\10"
if not exist "%SDKROOT%\Include" goto err_sdk

set "SDKVER="
for /f "usebackq delims=" %%D in (`dir /b /ad /o-n "%SDKROOT%\Include" 2^>nul`) do if not defined SDKVER set "SDKVER=%%D"
if not defined SDKVER goto err_sdk

set "MSVC=%VSROOT%\VC\Tools\MSVC\%MSVCVER%"
set "INCLUDE=%MSVC%\include;%MSVC%\atlmfc\include;%SDKROOT%\Include\%SDKVER%\ucrt;%SDKROOT%\Include\%SDKVER%\um;%SDKROOT%\Include\%SDKVER%\shared;%SDKROOT%\Include\%SDKVER%\winrt;%SDKROOT%\Include\%SDKVER%\cppwinrt"
set "LIB=%MSVC%\lib\x64;%MSVC%\atlmfc\lib\x64;%SDKROOT%\Lib\%SDKVER%\ucrt\x64;%SDKROOT%\Lib\%SDKVER%\um\x64"
set "PATH=%MSVC%\bin\Hostx64\x64;%SDKROOT%\bin\%SDKVER%\x64;%PATH%"

echo === GLFrame build  [%CFG%] ===
echo   toolset : %MSVCVER%
echo   sdk     : %SDKVER%
echo.

if exist "%OUT%" goto have_out
mkdir "%OUT%"
:have_out
if exist "%OBJ%" goto have_obj
mkdir "%OBJ%"
:have_obj

rem ---------------- per-configuration compiler flags ----------------------
if /i "%CFG%"=="Debug" goto cfg_debug
set "CFLAGS=/MD /O2 /DNDEBUG"
goto cfg_ready
:cfg_debug
set "CFLAGS=/MDd /Od /Zi /D_DEBUG"
:cfg_ready

rem ---------------- resources ---------------------------------------------
echo [1/2] Compiling resources...
rc /nologo /fo "%OBJ%\GLFrame.res" "%PROJ%\GLFrame.rc"
if errorlevel 1 goto err_rc

rem ---------------- compile + link ---------------------------------------
echo [2/2] Compiling and linking...
pushd "%PROJ%"
cl /nologo /W3 /EHsc /utf-8 /D_AFXDLL /D_WINDOWS /DUNICODE /D_UNICODE /D_CRT_SECURE_NO_WARNINGS /D_WIN32_WINNT=0x0601 %CFLAGS% /Fo"%OBJ%/" GLFrame.cpp MainFrm.cpp GLFrameDoc.cpp GLFrameView.cpp Sierpinski.cpp Framebuffer.cpp "%OBJ%\GLFrame.res" /Fe"%OUT%\GLFrame.exe" /link /SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup opengl32.lib glu32.lib
set "RC=%ERRORLEVEL%"
popd
if not "%RC%"=="0" goto err_compile
if not exist "%OUT%\GLFrame.exe" goto err_compile

rem ---------------- headless self-check screenshot ------------------------
rem The program renders one frame, saves it as BMP and exits by itself
rem when GLFRAME_CAPTURE is set (see Framebuffer.h).
echo.
echo [check] Rendering one frame to a BMP...
if not exist "%OUT%\selfcheck.bmp" goto run_check
del /q "%OUT%\selfcheck.bmp"
:run_check
set "GLFRAME_CAPTURE=%OUT%\selfcheck.bmp"
"%OUT%\GLFrame.exe"
set "GLFRAME_CAPTURE="
if not exist "%OUT%\selfcheck.bmp" goto err_selfcheck

echo.
echo [OK] Build succeeded.
echo      exe  : %OUT%\GLFrame.exe
echo      image: %OUT%\selfcheck.bmp
echo.
echo NOTE: this script does not open the interactive window.
echo       Run run.bat to launch the program.
echo %DATE% %TIME% BUILD OK - config=%CFG%>"%STATUS%"
goto done

:err_vs
echo [ERROR] Visual Studio 2022 with C++ tools was not found.
echo         Edit the VSROOT detection lines in this script.
echo %DATE% %TIME% BUILD FAILED - MSVC toolset not found>"%STATUS%"
goto done

:err_sdk
echo [ERROR] Windows 10/11 SDK was not found under
echo         "%SDKROOT%\Include".
echo %DATE% %TIME% BUILD FAILED - Windows SDK not found>"%STATUS%"
goto done

:err_rc
echo [FAILED] Resource compilation failed.
echo %DATE% %TIME% BUILD FAILED - rc.exe failed>"%STATUS%"
goto done

:err_compile
echo [FAILED] Compilation or linking failed.
echo %DATE% %TIME% BUILD FAILED - compile/link error>"%STATUS%"
goto done

:err_selfcheck
echo [FAILED] The program did not produce a screenshot.
echo          It probably failed to create an OpenGL rendering context.
echo %DATE% %TIME% BUILD FAILED - OpenGL self-check failed>"%STATUS%"
goto done

:done
if /I "%~2"=="nopause" goto end
echo.
pause
:end
endlocal
