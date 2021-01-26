# fileHasher

This is a project aimed to mimic the various *sum commands(e.g. `sha256sum`) and combine them. Currently it's build
through CMake and Conan.

## build

To build:

```sh
mkdir build && cd build
conan install ..  
conan install .. --build=missing
cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build .
```

Your desired binary lies in the bin/ directory then. Feel free and have fun to use it. :)
