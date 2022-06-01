Dependencies:
cmake
ninja
wget
binutils-dev

Assumes the system linker is the gold linker.  Recent versions (e.g. current release) of llvm, cmake, ninja
preferred.  

To build:
./configure.sh
make

For details, see the paper CFIXX: Object Type Integrity for C++ Virtual
Disaptch at NDSS '18.


cfixxv2:
compile programs using the clang++ from release install and set the following flags:
-O2 // required by cfixx
-fuse-ld=gold // use gold linker
-stdlib=libc++ // link against libc++ standard library, which causes the abi library that the standard library was linked against to be linked: libc++abi in this case

it is necessary to include the dir containing the standard library in the ld_library_path, which is searched to find dynamic dependencies at application startup
 export LD_LIBRARY_PATH=/home/marco/CFIXX-v2/release-install/lib
this can be set permantly in ~/.profile

the programm ldd can be used to check dynamic dependencies of a binary