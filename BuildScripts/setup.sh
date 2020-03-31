
QT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

#
# Clone the Qt5 sources
#
git clone git://code.qt.io/qt/qt5.git "$QT_DIR/sources"

#
# Init the submodules
#
pushd "$QT_DIR/sources"
git submodule init
popd
