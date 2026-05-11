default:
    @just --list

setup:
    cmake -S . -B build -D CMAKE_BUILD_TYPE=Debug

build-all:
    cmake --build build

run name:
    cmake --build build --target {{name}}
    ./build/{{name}}

alias r := run

clean:
    rm -rf build/

list:
    @just --list

help: list 