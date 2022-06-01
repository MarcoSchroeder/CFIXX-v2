../release-install/bin/clang++ src/metadata_attack.cpp src/attack_typeid.cpp -o build/cfixx_clang_metadata_attack -O2 -g -std=c++11 -v -fuse-ld=gold -stdlib=libc++

if [ $? -eq 0 ]
then
  echo "Build success. Running executable..."
  echo ""
  ./build/cfixx_clang_metadata_attack
else
  echo "Build failure. Aborting..." >&2
  exit 1
fi