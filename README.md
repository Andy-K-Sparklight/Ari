# Alicorn: The Corrupted Heart

## Overview

ACH is aimed to be a launcher with high performance, high reliability and an easygoing user interface, which is now under development.

## Build Instructions

**NOTE:** If you don't want to read the full document, please just use the binary release. The build of ACH is quite different.

**NOTE:** ACH only supports Autotools and GCC currently. Make sure your compiler supports C++23 before continuing.

Basically, the build of ACH contains two parts: the JS part and the C++ part. Please bear that in mind.

### Prerequisites For All Platforms

To build ACH, you need a C/C++ toolchain and a working Node.js installation.

Remember to have `yarn` installed when installing Node.js. If you've forgotten that, run:

```bash
npm install -g yarn
```

### Windows Prerequisites

To build on Windows, you must have MSYS2 installed. It provides a variety of necessary tools.

You'll need to install the following packages by running a simple command:

```bash
pacman -S git autotools make mingw-w64-x86_64-gcc mingw-w64-x86_64-openssl mingw-w64-x86_64-zlib
```

Make sure to start terminal **MINGW64**, i.e.:

```bash
echo $MSYSTEM
```

Should output `MINGW64`.

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

According to our CI, you also need to install Xcode version 14.1 beta 3 or any newer version to avoid LD bugs.

Then we're ready.

### GNU/Linux Prerequisites

Most tools have already been set up on GNU/Linux. However, there are still quite a few.

You probably need to install:

```bash
apt install libwebkit2gtk-4.0-dev libgtk-3-dev libuv1-dev pkg-config
```

These packages might also be available on other PMs.

Make sure also to update your other packages to the latest version, including `gcc`.

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

   If you are using macOS, `configure` might complain about the built-in `make`. If this is the case, run `./configure MAKE=gmake` as the output says.

3. Run the build.

   For macOS:

   ```bash
   gmake CC=gcc-12 CPP=g++-12 CXX=g++-12 LD=g++-12
   ```

   If you've got a later gcc version available, just change the suffix to the proper one.

   For other systems, just:

   ```bash
   make
   ```

   If you want the build to be faster, append flag `-jX` where X is the same as the count of your CPU cores.

4. If building for Windows, run:

   ```bash
   ./winfix
   ```

5. Now build the JS part:

   ```bash
   yarn install
   chmod +x ./jsbuild
   ./jsbuild
   cp ./dist/* ./
   ```

6. Make sure to copy the assets we needed:

   ```
   cp -r ./assets/* ./
   ```

7. The binary output `AlicornCH.exe` or `AlicornCH` is now available under your CWD, just run it and try!

## Licensing

Copyright (C) 2022-2023 A2 Craftware Tek MC.

This program is distributed under the GNU General Public License (Version 3). If a later version is available, then the latest one applies.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
