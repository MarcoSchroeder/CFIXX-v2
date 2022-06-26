g++ src/main_dynamic_cast_complex_scenario.cpp  -o build/gcc_dynamic_cast_complex -O2 -g -std=c++11

if [ $? -eq 0 ]
then
  echo "Build success. Running executable..."
  echo ""
  ./build/gcc_dynamic_cast_complex
else
  echo "Build failure. Aborting..." >&2
  exit 1
fi