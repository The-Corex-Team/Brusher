cd ~/Projects/Brusher
rm -rf build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
./build/Brusher