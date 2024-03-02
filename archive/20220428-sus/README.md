# sus
Compiler for the [Among Us programming language](https://esolangs.org/wiki/Among_Us). It uses C as it's native backend and it optimizes code automatically, so no `-O` flags are required.

## Download

Prebuilt (64-bit) binaries are available.

- [Windows](https://github.com/null8626/sus/releases/download/v1.0.0/sus-windows.zip)
- [Linux](https://github.com/null8626/sus/releases/download/v1.0.0/sus-linux.zip)

## Hello World example

Try to compile the `helloworld.sus` file in the root of this repository, and it should print out `Hello, world!`!

Compile to an executable with:
```sh
$ ./sus ./helloworld.sus
$ ./helloworld
Hello, world!
```

Or to output as a C source:
```sh
$ ./sus ./helloworld.sus -o ./helloworld.c
$ cat ./helloworld.c
# ...
```
## Building locally

### Windows (MSVC)
```bat
build
```

### Windows (GCC)
```bat
build gcc
```

### Windows (Clang)
Works if you either installed this [directly from the official repo's releases](https://github.com/llvm/llvm-project/releases/) or [the one from Visual Studio](https://www.wikihow.com/Install-Clang-on-Windows).

```bat
build clang
```

### Linux (GCC)
```sh
chmod +x ./build
./build
```

### Linux (Clang)
```sh
chmod +x ./build
./build clang
```

## Testing
This will build the hello world file and tests if the compiler works.

**IMPORTANT NOTES:**
- Run this directly after building from source, thus the compiler executable must be in the same directory. (aka project root)
- This requires Node.js v18 or later.

```sh
npm i fast-fail
node ./test.mjs
```
