#! /usr/bin/bash

if [ ! -d "build" ]; then
  mkdir build
fi

run_after_build=false

while [[ "$#" -gt 0 ]]; do
    case $1 in
        -debug|-d) build_type="debug" ;;
        -release|-r) build_type="release" ;;
        -profile|-p) build_type="profile" ;;
        -run) run_after_build=true ;;
        *) echo "Unknown option: $1, usage: ./build.sh [d(ebug)|r(elease)]"  exit 1 ;;
    esac
    shift
done

cd build

if [ "$build_type" == "debug" ]; then
    cmake -DCMAKE_BUILD_TYPE=Debug --log-level=WARNING ..
    cmake --build . 
elif [ "$build_type" == "release" ]; then
    cmake -DCMAKE_BUILD_TYPE=Release --log-level=WARNING ..
    cmake --build . 
elif [ "$build_type" == "profile" ]; then
    cmake -DCMAKE_BUILD_TYPE=Profile --log-level=WARNING ..
    cmake --build .     
fi

if [ $? -eq 0 ]; then
    if [ "$run_after_build" = true ]; then
        ./path_tracer_app
    fi
fi

