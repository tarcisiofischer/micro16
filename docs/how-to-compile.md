## How to compile

Given you have all dependencies installed (SDL2. The others are copied in the
source), then you just need to run:

```
$ mkdir build/ && cd build/
$ cmake ..
$ cmake --build . -j$(nproc)
```

Note: In debian-based systems such as Ubuntu, SDL2 is available in the package
manager, you can install it with

```
$ sudo apt install libsdl2-dev
```

SDL2 is also available with `Conan` and other package managers.

You can run tests using `ctest` from the build folder:

```
$ ctest
```

There are example executables in the `examples/` folder. You can run `micro16` as below, from the build folder:

```
$ ./src/micro16 ../examples/led_blink.micro16
```

