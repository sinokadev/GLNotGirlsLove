default: build run

setup:
    cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug

build:
    cmake --build build

# 실행
run:
    ./build/GLNotGirlsLove

exec:
    ./build/GLNotGirlsLove

clean:
    rm -rf build/

list:
    @just --list