../release-install/bin/clang++ src/main_typeid.cpp src/attack_typeid.cpp -o build/cfixx_clang_typeid -O2 -g -std=c++11

if [ $? -eq 0 ]
then
  echo "Build success. Running executable..."
  echo ""
  ./build/cfixx_clang_typeid
else
  echo "Build failure. Aborting..." >&2
  exit 1
fi