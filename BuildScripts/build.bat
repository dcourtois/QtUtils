@echo off

::
:: Configure Visual Studio 2019 Community for x64 build.
::
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

::
:: Go to the build directory
::
set QT_DIR=%~dp0
if not exist "%QT_DIR%\build" ( mkdir "%QT_DIR%\build" )
pushd "%QT_DIR%\build"

::
:: Configure Qt.
::
call %QT_DIR%\sources\configure           ^
    -release                              ^
    -opensource -confirm-license          ^
    -prefix "%QT_DIR%/install"            ^
    -platform win32-msvc                  ^
    -mp                                   ^
    -c++std c++17                         ^
    -silent                               ^
    -nomake examples                      ^
    -nomake tests                         ^
    -no-assimp                            ^
    -no-feature-assimp                    ^
    -no-harfbuzz                          ^
    %*

::
:: Build and install
::
nmake
nmake install

::
:: Restore the previous working directory
::
popd
