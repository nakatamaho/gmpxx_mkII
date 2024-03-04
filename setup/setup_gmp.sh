VERSION=6.3.0
export CC=gcc-12
export CXX=g++-12

WRKDIR=/home/docker/gmpxx_mkII/setup/work_gmp
ARCHIVEDIR=/home/docker/gmpxx_mkII/archives

rm -rf $WRKDIR
mkdir -p $WRKDIR
cd $WRKDIR
tar xvf $ARCHIVEDIR/gmp-${VERSION}.tar.xz
cd gmp-${VERSION}
./configure --enable-cxx --prefix=/home/docker/gmpxx_mkII/i/GMP-${VERSION}
make -j`nproc`
make install

cd ..
rm -rf $WRKDIR
