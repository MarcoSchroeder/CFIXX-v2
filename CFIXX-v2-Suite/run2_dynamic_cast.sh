../release-install/bin/clang++ src/main_dynamic_cast.cpp src/attack_dynamic_cast.cpp -o build/cfixx_clang_dynamic_cast -O2 -g -std=c++11 -v -fuse-ld=gold -stdlib=libc++

if [ $? -eq 0 ]
then
  echo "Build success. Running executable..."
  echo ""
  ./build/cfixx_clang_dynamic_cast
else
  echo "Build failure. Aborting..." >&2
  exit 1
fi