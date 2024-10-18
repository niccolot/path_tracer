#! /usr/bin/bash

if [ ! -d "build" ]; then
  mkdir build
fi

run_after_build=false

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -debug|-d) build_type="debug" ;;
        -release|-r) build_type="release" ;;
        -run) run_after_build=true ;;
        *) echo "Unknown option: $1, usage: ./build.sh [d(ebug)|r(elease)]"  exit 1 ;;
    esac
    shift
done

cd build

if [ "$build_type" == "debug" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug ..
    cmake --build . 
elif [ "$build_type" == "release" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release .
    cmake --build . 
fi

if [ $? -eq 0 ]; then
    if [ "$run_after_build" = true ]; then
        ./path_tracer  > image.ppm
    fi
fi