echo -en 'travis_fold:start:SLF7ContainerSetup\r'
echo "SLF7 Container Setup"

yum -y install epel-release
yum -y install make base-devel glibc-devel freetype-devel xxhash-devel xxhash-libs libcurl libcurl-devel libzstd-devel libzstd

#python3 -m pip install scons

source /cvmfs/fermilab.opensciencegrid.org/products/common/etc/setups
setup mu2e
setup root v6_20_06 -f Linux64bit+3.10-2.17 -q e19:p383b:prof
#setup -B scons v3_1_2 -q +p383b


echo -en 'travis_fold:end:SLF7ContainerSetup\r'

echo -en 'travis_fold:start:Build\r'
echo "Build"

rm -rf build || echo ""
mkdir build && cd build

#source ../scripts/newBuild.sh prof
#source setup.sh

#scons -j 4

#scons test


setup cmake v3_17_3

cmake .. -DCMAKE_BUILD_TYPE=Release

make -j 8

echo -en 'travis_fold:end:Build\r'

make test 

