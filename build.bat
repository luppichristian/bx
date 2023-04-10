@echo off

if not exist .build mkdir .build
pushd .build

cl ../bx_test.cpp /nologo /FC /Ob0 /Od /Z7 /link /incremental:no user32.lib gdi32.lib

popd