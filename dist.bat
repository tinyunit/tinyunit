cd /d %~dp0
set WD=%CD%
rd /s /q build-dist
mkdir build-dist
cd build-dist
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

cmake -G Ninja .. -DCMAKE_INSTALL_PREFIX=%WD%\..\dist -DBUILD_TESTING:BOOL=TRUE -DSTATIC_RUNTIME:BOOL=TRUE -DCMAKE_BUILD_TYPE=RelWithDebInfo
ninja install
pause
