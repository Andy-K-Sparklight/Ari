# Alicorn: The Corrupted Heart

## Overview

ACH is aimed to be a launcher with high performance, high reliability and an easygoing user interface, which is now under development. 

## Licensing

All Rights Reserved. 

## Build Instructions

ACH currently supports GNU/Linux and macOS. Windows build support with MSYS2 might work but not guaranteed.

You have to install dependencies such as [libuv](https://github.com/libuv/libuv), gcc, autoconf, openssl. 

**You need a compiler which supports GNU++23.**

For macOS:

```Shell
brew install libuv gcc autoconf openssl@1.1
```

Then it can be built by AutoTools.

For MSYS2 on Windows and GNU:

```Shell
git clone --depth=1 https://github.com/Andy-K-Sparklight/ACH.git
autoreconf --install
./configure
make
```

For macOS:
**You have to specify the compiler, or it will use Apple Clang as compiler, that may cause the build process to fail. **
```Shell
git clone --depth=1 https://github.com/Andy-K-Sparklight/ACH.git
autoreconf --install
./configure
make CC=gcc-<version> CPP=g++-<version> CXX=g++-<version> LD=g++-<version>
```

Up to now, the command is:

```Shell
make CC=gcc-12 CPP=g++-12 CXX=g++-12 LD=g++-12
```
