@echo off
setlocal

pushd "%~dp0"

call build_math_engine.cmd
if errorlevel 1 exit /b %errorlevel%

call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b %errorlevel%

if not exist build\app mkdir build\app

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DUNICODE /D_UNICODE /I. /Fe"build\app\MathEngineRunner.exe" MathEngineRunner.cpp /link /LIBPATH:"build\bin" MathEngine.lib
if errorlevel 1 exit /b %errorlevel%

copy /Y "build\bin\MathEngine.dll" "build\app\MathEngine.dll" >nul
if errorlevel 1 exit /b %errorlevel%

echo Running MathEngineRunner.exe...
"build\app\MathEngineRunner.exe"

popd
endlocal
