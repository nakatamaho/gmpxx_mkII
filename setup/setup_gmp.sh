#!/bin/bash

set -e  # Exit on error

VERSION=6.3.0
export CC=gcc-13
export CXX=g++-13
WRKDIR=/home/docker/gmpxx_mkII/setup/work_gmp
ARCHIVEDIR=/home/docker/gmpxx_mkII/archives
INSTALLDIR=/home/docker/gmpxx_mkII/i/GMP-${VERSION}
ARCHIVE_FILE="$ARCHIVEDIR/gmp-${VERSION}.tar.xz"

# Basic checks
echo "Checking prerequisites..."
command -v $CC >/dev/null || { echo "Error: $CC not found"; exit 1; }
command -v $CXX >/dev/null || { echo "Error: $CXX not found"; exit 1; }
[[ -f "$ARCHIVE_FILE" ]] || { echo "Error: Archive not found: $ARCHIVE_FILE"; exit 1; }

echo "Building GMP $VERSION..."

# Clean and setup
rm -rf $WRKDIR
mkdir -p $WRKDIR $INSTALLDIR

cd $WRKDIR
echo "Extracting..."
tar xf $ARCHIVE_FILE

cd gmp-${VERSION}
echo "Configuring..."
./configure --enable-cxx --enable-shared --enable-static --prefix=$INSTALLDIR

echo "Building..."
make -j`nproc`

echo "Installing..."
make install

# Cleanup
cd /
rm -rf $WRKDIR

echo "GMP $VERSION installed to: $INSTALLDIR"
echo "Add to environment: export PKG_CONFIG_PATH=\"$INSTALLDIR/lib/pkgconfig:\$PKG_CONFIG_PATH\""
