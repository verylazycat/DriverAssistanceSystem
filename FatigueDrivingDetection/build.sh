#!bin/bash
cd build/
cmake .. -DCMAKE_BUILD_TYPE=Release | tee ../log/cmake_log
make -j8 |tee ../log/make_log
