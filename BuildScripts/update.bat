@echo off

set QT_DIR=%~dp0
pushd "%QT_DIR%/sources"

::
:: Checkout
::
call git checkout %*

::
:: Update
::
call git pull
call git submodule update

popd
