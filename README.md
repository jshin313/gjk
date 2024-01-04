# GJK Collision Detection
Implementation of GJK algorithm for collision detection for native builds, WebASM, and the TI-84+ CE

![Animated png of the GJK collision working with two polygons on a TI-84+ CE calculator](screenshot.png)

## Building
* Native builds: `make`
* WebAssembly: `make wasm` and then run `./public/serve.sh` to host the wasm files
* TI-84+ CE: `make ti` and transfer the `bin/GJK.8xp` file to your calculator

## Dependencies
* Native
    * SDL2
    * SDL2_gfx
```
 sudo apt-get install libsdl2-dev
 sudo apt-get install libsdl2-gfx-dev
```

* WebASM
    * [emcc](https://emscripten.org/docs/getting_started/downloads.html)
* TI-84+ CE
    * [CE Toolchain](https://ce-programming.github.io/toolchain/static/getting-started.html)
    * Transfer clibs to your calculator: https://tiny.cc/clibs

## Directories
* `public`: WebASM `index.html` and `index.js`
    * `serve.sh` starts a simple server to start the WebASM
* `bin`: Native binary and 8xp files
* `src`: Source code

## Resources
`src/gjk/gjk.c` is heavily based on the following resources:
 * https://youtu.be/ajv46BSqcK4?si=FwvnEZ4qwEg9061W&t=1756
    * Very nice explanation on how GJK works
 * https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
    * More comprehensive examples with concrete numbers and calculations
 * https://dyn4j.org/2010/04/gjk-distance-closest-points/
    * How to also get the closest points on two polygons with GJK
