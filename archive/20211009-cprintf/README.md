# cprintf

Tiny and cross-platform colored `printf()`.

Include this:
```c
// uncomment next line if you're using the DLL
// #define CPRINTF_DLL

#include <cprintf.h>
```
Sample uses:
```c
// assume you are a part of the majority who use a
// terminal with a black background and a white text...

cprintf(NULL, "What's your favorite color? %fr\n", "red!");
cprintf(NULL, "What's your favorite color? %fg\n", "green!");
cprintf(NULL, "What's your favorite color? %fb\n\n", "blue!");

cprintf(
  NULL,
  
  "What's your favorite color? %FW\n"
  "God, that was a BRIGHT white!\n\n",
  
  "WHITE!"
);

cprintf(
  NULL,
  
  "What does CMYK stand for?\n"
  "%fc, %fm, %fy, and %fk\n"
  "wait, i can't see the last one! %i\n"
  "Oh, there we go! Thanks!\n\n",
  
  "cyan", "magenta", "yellow", "key (black)",
  
  "i said black! here, "
  "i have inverted the background so you can see me!"
);

cprintf(NULL, "What's your favorite background color? %br\n", "red!");
cprintf(NULL, "I have underlined %u so you can understand it.\n\n", "this");

cprintf(
  NULL,
  "What's your favorite mode(s) in cprintf? %{u,FR,BG}\n\n",
  "Underlined bright red text in a bright green background!"
);

cprintf(NULL, "Oh, God! i have an %FR! switching to error mode...\n", "ERROR");
cprintf_use(NULL, CPRINTF_STDERR);
cprintf(NULL, "%{BR,FW}\n", "ERROR! ERROR! WEE WOO WEE WOO!");
cprintf_use(NULL, CPRINTF_STDOUT);
cprintf(NULL, "Okay, back to %FG Yay!\n\n", "normal!");

cprintf(NULL, "Mommy, look! An ampersand! --> %%\n");
cprintf(NULL, "Wait, what's that %{i,u} in the beginning for?\n\n", "\"NULL\"");

cprintf_t my_handle;
cprintf_use(&my_handle, CPRINTF_STDOUT);
cprintf(&my_handle, "My own %{u,FW} let's goo!\n", "handle!");
```

# Download

Prebuilt (64-bit) binaries are available.

- [Windows](https://github.com/null8626/cprintf/releases/download/v1.1.0/cprintf-windows.zip)
- [macOS](https://github.com/null8626/cprintf/releases/download/v1.1.0/cprintf-macos.zip)
- [Linux](https://github.com/null8626/cprintf/releases/download/v1.1.0/cprintf-linux.zip)

# Building with CMake

To create the build files:

```console
cmake -B build .
cmake build
```

And then:

- For POSIX:

```console
cd build
make
```

- For Windows:

```console
msbuild build/cprintf.sln -noLogo -p:Configuration=Release
```

And the library files should be there in the `build/Release` directory.
