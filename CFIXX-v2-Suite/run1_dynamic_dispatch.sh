../release-install/bin/clang++ src/main_dynamic_dispatch.cpp src/attack_dynamic_dispatch.cpp -o build/cfixx_clang_dynamic_dispatch -O2 -g -std=c++11 -v -fuse-ld=gold -stdlib=libc++

if [ $? -eq 0 ]
then
  echo "Build success. Running executable..."
  echo ""
  ./build/cfixx_clang_dynamic_dispatch
else
  echo "Build failure. Aborting..." >&2
  exit 1
fi