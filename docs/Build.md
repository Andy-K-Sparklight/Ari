# Build Instructions

## Important Notes

**NOTE:** Ari only supports Autotools and GCC currently. Make sure your compiler supports C++23 before continuing.

Basically, the build of Ari contains two parts: the JS part and the C++ part. Please bear that in mind.

## Prerequisites For All Platforms

To build Ari, you need a C/C++ toolchain and a working Node.js installation.

Remember to have `yarn` installed when installing Node.js. If you've forgotten that, run:

```bash
npm install -g yarn
```

## Windows Prerequisites

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

## Mac Prerequisites

To build on macOS, you need the following packages:

```bash
brew install libuv gcc@12 autoconf openssl@1.1
```

If you are building for ARM64, please also link openssl:

```bash
brew link openssl@1.1 --force
```

Make sure you've also got `clang` installed to compile some codebase with Apple's extensions.

According to our CI, you also need to install Xcode version 14.1 beta 3 or any newer version to avoid LD bugs.

Then we're ready.

## GNU/Linux Prerequisites

Most tools have already been set up on GNU/Linux. However, there are still quite a few.

You probably need to install:

```bash
apt install libwebkit2gtk-4.0-dev libgtk-3-dev libuv1-dev pkg-config
```

These packages might also be available on other PMs.

Make sure also to update your other packages to the latest version, including `gcc`.

## Build Automatically

Since commit [9e963cf](https://github.com/Andy-K-Sparklight/Ari/commit/9e963cf1776827fcf37450e181591cd58f157c42) an automatic build script has been added. What it does are just the manually build steps.

Before using it, please make sure all requirements (compilers, node, libraries, etc.) have been satisfied, or the script will not work.

This script is also **bash compatible only**, be aware if your `/bin/sh` is not bash compatible.

1. First, clone the repository:

   ```shell
   git clone https://github.com/Andy-K-Sparklight/Ari.git --depth 1
   cd Ari
   ```

2. Give permission to `./shigure` and run the build.

   ```shell
   chmod +x ./shigure
   ./shigure setup
   ./shigure build
   ./shigure test
   ```

If everything works, the product binary should be available at `./build/Ari` (With the suffix `.exe` on Windows).

If something seems to be wrong, continue to read the following section and build manually.

## Build Manually

1. First, clone the repository:

   ```bash
   git clone https://github.com/Andy-K-Sparklight/Ari.git --depth 1
   cd Ari
   ```

2. Download some binaries:

   Some files need to be downloaded during build time. Make sure you've got `wget` or `curl` available in your `PATH` to do so.

   ```bash
   chmod +x ./shigure
   ./shigure setup
   ```

3. Run the configure script:

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

4. Run the build.

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

5. If building for Windows, run:

   ```bash
   ./winfix
   ```

6. Now build the JS part:

   ```bash
   yarn install
   chmod +x ./jsbuild
   ./jsbuild
   cp ./dist/* ./
   ```

7. Make sure to copy the assets we needed:

   ```
   cp -r ./assets/* ./
   ```

8. The binary output `Ari.exe` or `Ari` is now available under your CWD, just run it and try!
