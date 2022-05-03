# Run from CFIXX-v2 root directory

# These fixes come from here: https://github.com/google/sanitizers/issues/822
cp build_fix/sanitizer_stoptheworld_linux_libcdep.cc llvm/projects/compiler-rt/lib/sanitizer_common/sanitizer_stoptheworld_linux_libcdep.cc
cp build_fix/sanitizer_linux.h llvm/projects/compiler-rt/lib/sanitizer_common/sanitizer_linux.h
cp build_fix/tsan_platform_linux.cc llvm/projects/compiler-rt/lib/tsan/rtl/tsan_platform_linux.cc
cp build_fix/esan_sideline_linux.cpp llvm/projects/compiler-rt/lib/esan/esan_sideline_linux.cpp
cp build_fix/sanitizer_linux.cc llvm/projects/compiler-rt/lib/sanitizer_common/sanitizer_linux.cc

# This fix just comments out the include of the header file "xlocale.h"
cp build_fix/__locale libcxx/include/__locale