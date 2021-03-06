# !/bin/bash

LIBRARY="cgns-3.3.1"
export FLIBS="-Wl,--no-as-needed -ldl -lz"
export LIBS="-Wl,--no-as-needed -ldl -lz"
export CLIBS="-ldl"

if [ ! -d "$LIBRARY" ]; then
    git clone -b master "https://github.com/CGNS/CGNS.git"
    mv CGNS $LIBRARY
fi

cd $LIBRARY
cd src

CGNS_CONFIGURE_FLAG="--enable-debug"
if [ "${BUILD_TYPE^^}" == "RELEASE" ]; then
    CGNS_CONFIGURE_FLAG="--disable-debug"
fi

./configure --without-fortran --disable-cgnstools --enable-shared $CGNS_CONFIGURE_FLAG --prefix=$LIBRARY_INSTALL_DIRECTORY/$LIBRARY/$BUILD_TYPE

make -j 2

make install
