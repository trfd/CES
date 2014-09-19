wd=`pwd`
apt-get install -y libgtest-dev
cd /usr/src/gtest
cmake
make
cp *.a /usr/lib
cd $wd