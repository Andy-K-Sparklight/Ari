# Alicorn: The Corrupted Heart

## Overview

ACH is aimed to be a launcher with high performance, high reliability and an easygoing user interface, which is now under development. 

## Licensing

All Rights Reserved. 

## Build Instructions

**NOTE:** If you don't want to read the full document, please just use the binary release. The build of ACH is quite different.

### Windows Prerequisites

To build on Windows, you must have MSYS2 installed. It provides a variety of necessary tools.

You'll need to install the following packages, a simple command:

```bash
pacman -S git autotools make mingw-w64-x86_64-gcc mingw-w64-x86_64-openssl mingw-w64-x86_64-zlib
```

Make sure to start terminal **MINGW64**, which means:

```bash
echo $MSYSTEM
```

Must be `MINGW64`.

Keep yourself in this terminal and we're ready.

### Mac Prerequisites

To build on macOS, you need the following packages:

```bash
brew install libuv gcc autoconf openssl@1.1
```

If you are building for ARM64, please also link openssl:

```bash
brew link openssl@1.1 --force
```

Make sure you've also got `clang` installed to compile some codebase with Apple's extensions.

Then we're ready.

### GNU/Linux Prerequisites

Most tools have already been set up on GNU/Linux. However, there are still quite a few.

First, you need a compiler which supports **GNU++23**.

And then, you probably need to install:

```bash
apt install libwebkit2gtk-4.0-dev libgtk-3-dev libuv1-dev
```

These packages might also be available on other PMs.

### Build

1. First, clone the repository:
   
   ```bash
   git clone https://github.com/Andy-K-Sparklight/ACH.git --depth 1
   cd ACH
   ```

2. Run the configure script:
   
   For macOS please first run:
   
   ```bash
   chmod +x ./macpb
   ./macpb
   ```
   
   Then for all platforms, run:

   ```bash
   autoreconf --install
   ./configure
   ```
   
   If you are using macOS, `configure` might complain about the built-in `make`. If this is the case, do as the output say.

3. Run the build.
   
   For macOS:
   
   ```bash
   make CC=gcc-12 CPP=g++-12 CXX=g++-12 LD=g++-12
   ```
   
   If you've got a later gcc available, just change the suffix to a proper one. Also, if any complains exists, use `gmake` and try again.
   
   For other systems, just:
   
   ```bash
   make
   ```
   
   If you want the build to be faster, append flag `-jX` where X is the same as your CPU cores number.

4. If building for Windows, run:
   
   ```bash
   ./winfix
   ```

5. The binary output `AlicornCH.exe` is now available under your CWD, just run it and try!
