../release-install/bin/clang++ src/main_all.cpp src/attack_dynamic_dispatch.cpp src/attack_dynamic_cast.cpp src/attack_typeid.cpp -o build/cfixx_clang_all -O2 -g -std=c++11

if [ $? -eq 0 ]
then
  echo "Build success. Running executable..."
  echo ""
  ./build/cfixx_clang_all
else
  echo "Build failure. Aborting..." >&2
  exit 1
fi