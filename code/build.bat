@echo off

pushd .
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86
popd

if not exist ..\build mkdir ..\build
pushd ..\build
del *.pdb 1> NUL 2> NUL

set disabled_warnings=-wd4127 -wd4996 -wd4100 -wd4189 -wd4100 -wd4505 -wd4706
set compiler_flags=-nologo /I ../code/vendor/imgui -W4 -WX %disabled_warnings% /Wv:18 -nologo -Z7 /EHsc -Od
set linker_flags=-incremental:no User32.lib Gdi32.lib D3DCompiler.lib D3d11.lib D3d12.lib DXGI.lib DSound.lib winmm.lib

cl %compiler_flags% ..\code\Infinitum.cpp /link %linker_flags%

popd
cd ..\build

