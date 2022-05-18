# Run from CFIXX-v2 root directory

# These fixes come from here: https://github.com/google/sanitizers/issues/822
cp CFIXX-v2-Build-Fix/sanitizer_stoptheworld_linux_libcdep.cc llvm/projects/compiler-rt/lib/sanitizer_common/sanitizer_stoptheworld_linux_libcdep.cc
cp CFIXX-v2-Build-Fix/sanitizer_linux.h llvm/projects/compiler-rt/lib/sanitizer_common/sanitizer_linux.h
cp CFIXX-v2-Build-Fix/tsan_platform_linux.cc llvm/projects/compiler-rt/lib/tsan/rtl/tsan_platform_linux.cc
cp CFIXX-v2-Build-Fix/esan_sideline_linux.cpp llvm/projects/compiler-rt/lib/esan/esan_sideline_linux.cpp
cp CFIXX-v2-Build-Fix/sanitizer_linux.cc llvm/projects/compiler-rt/lib/sanitizer_common/sanitizer_linux.cc

# This fix just comments out the include of the header file "xlocale.h"
cp CFIXX-v2-Build-Fix/__locale libcxx/include/__locale