
QT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
pushd "$QT_DIR/sources"

#
# Checkout
#
git checkout $@

#
# Update
#
git pull
git submodule update

popd
