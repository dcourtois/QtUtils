
QT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

#
# Go to the build directory
#
if [ ! -d "$QT_DIR/build" ]; then
    mkdir "$QT_DIR/build"
fi;
pushd "$QT_DIR/build"

#
# Configure Qt.
#
$QT_DIR/sources/configure           \
    -release                        \
    -opensource -confirm-license    \
    -prefix "$QT_DIR/install"       \
    -c++std c++17                   \
    -silent                         \
    -nomake examples                \
    -nomake tests                   \
    -no-assimp                      \
    -openssl                        \
    -skip qt3d                      \
    -skip qtdocgallery              \
    -skip qtfeedback                \
    $@

#
# Build and install
#
make -j
make install

#
# Restore the previous working directory
#
popd
