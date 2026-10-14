@echo off
REM Builds tools-side luacheck.exe against the repo's lua51.lib.
REM Mode flags MUST match luajit.vcxproj (LUAJIT_DISABLE_GC64) or the
REM link/run will misbehave. Output goes to %TEMP%\opencode (not the repo).
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >NUL
cd /d "%~dp0"
set LJINC=..\..\src\3rd party\luajit-2\src
set LJLIB=C:\Users\Gamer\source\repos\xray-monolith\_build\_game\bin_dbg\x64\Release\lua51.lib
if not exist "%LJLIB%" (
  echo [luacheck] lua51.lib not found at %LJLIB% - build the DX11/Release solution first.
  exit /b 1
)
if not exist "%TEMP%\opencode" mkdir "%TEMP%\opencode"
cl /nologo /MD /O2 /EHsc- /DLUAJIT_DISABLE_GC64 /I"%LJINC%" /Foluacheck.obj /Fe"%TEMP%\opencode\luacheck.exe" luacheck.c "%LJLIB%"
if errorlevel 1 goto fail
echo [luacheck] built: %TEMP%\opencode\luacheck.exe
goto end
:fail
echo [luacheck] BUILD FAILED
exit /b 1
:end
