rm -rf build

mkdir build

cd build

../configure --target-list=aarch64-softmmu --enable-debug

./build_and_run.sh