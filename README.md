# GJK Collision Detection
Implementation of GJK algorithm for collision detection

![Animated png of the GJK collision working with two polygons on a TI-84+ CE calculator](screenshot.png)

## Building
* For native builds: `make`
* For WebAssembly: `make wasm` and then run `./public/serve.sh` to host the wasm files
* For the TI-84+ CE: `make ti` and transfer the **GJK.8xp** file to your calculator

## Directories
* `public`: WebASM `index.html` and `index.js`
    * `serve.sh` starts a simple server to start the WebASM
* `bin`: Native binary and 8xp files
* `src`: Source code
