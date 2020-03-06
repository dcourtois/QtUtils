@echo off

set QT_DIR=%~dp0
pushd "%QT_DIR%/sources"

::
:: Checkout
::
call git checkout %*

::
:: Update the submodules
::
call git submodule update

popd
