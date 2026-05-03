@echo off
setlocal

call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b %errorlevel%

if not exist build\obj mkdir build\obj
if not exist build\bin mkdir build\bin

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yc"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\pch.obj" /Fd"build\obj\vc145.pdb" /c pch.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\AdvancedComputation.obj" /Fd"build\obj\vc145.pdb" /c AdvancedComputation.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\AlgebraStats.obj" /Fd"build\obj\vc145.pdb" /c AlgebraStats.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\dllmain.obj" /Fd"build\obj\vc145.pdb" /c dllmain.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\Geometry3D.obj" /Fd"build\obj\vc145.pdb" /c Geometry3D.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\MathCore.obj" /Fd"build\obj\vc145.pdb" /c MathCore.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\MathEngineExports.obj" /Fd"build\obj\vc145.pdb" /c MathEngineExports.cpp
if errorlevel 1 exit /b %errorlevel%

cl /nologo /W3 /EHsc /MD /O2 /std:c++20 /DNDEBUG /DTHEEXTRAHARLEVELMATHSPHDLEVEL_EXPORTS /D_WINDOWS /D_USRDLL /D_WINDLL /DUNICODE /D_UNICODE /Yu"pch.h" /Fp"build\obj\MathEngine.pch" /Fo"build\obj\SymbolicCalculus.obj" /Fd"build\obj\vc145.pdb" /c SymbolicCalculus.cpp
if errorlevel 1 exit /b %errorlevel%

link /nologo /DLL /OUT:"build\bin\MathEngine.dll" /IMPLIB:"build\bin\MathEngine.lib" build\obj\pch.obj build\obj\AdvancedComputation.obj build\obj\AlgebraStats.obj build\obj\dllmain.obj build\obj\Geometry3D.obj build\obj\MathCore.obj build\obj\MathEngineExports.obj build\obj\SymbolicCalculus.obj
if errorlevel 1 exit /b %errorlevel%

endlocal
