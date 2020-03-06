@echo off

set QT_DIR=%~dp0

::
:: Clone the Qt5 sources
::
call git clone git://code.qt.io/qt/qt5.git "%QT_DIR%/sources"

::
:: Init the submodules
::
pushd "%QT_DIR%/sources"
call git submodule init
popd
