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

## Resources
`src/gjk/gjk.c` is heavily based on the following resources:
 * https://youtu.be/ajv46BSqcK4?si=FwvnEZ4qwEg9061W&t=1756
    * Very nice explanation on how GJK works
 * https://dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
    * More comprehensive examples with numbers
 * https://dyn4j.org/2010/04/gjk-distance-closest-points/
    * How to also get the closest points on two polygons with GJK
