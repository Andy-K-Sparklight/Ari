# Alicorn: The Corrupted Heart

## Overview

ACH is aimed to be a launcher with high performance, high reliability and an easygoing User Interface, which is now under development. 

## Licensing

All Rights Reserved. 

## Build Instructions

There is no support for non-Unix-like platforms. 

Since ACH is based on [libuv](https://github.com/libuv/libuv), you have to install the dependencies with following commands:

For MinGW64 on Windows and GNU(Take Arch Linux as example):

```Shell
$ sudo pacman -S libuv gcc autoconf automake openssl
```

Users of other GNU releases can also install libuv from source.

For macOS:

```Shell
$ brew install libuv gcc autoconf automake openssl@1.1
```

Then it can be built by AutoTools.

For MinGW64 on Windows and GNU:

```Shell
$ git clone https://github.com/Andy-K-Sparklight/ACH.git
$ autoreconf --install
$ ./configure
$ make
```

For macOS:
```Shell
$ git clone https://github.com/Andy-K-Sparklight/ACH.git
$ autoreconf --install
$ ./configure
$ make CC=** CPP=** CXX=** LD=**
```

You have to specify the compiler, or it will use Apple Clang as compiler, that may cause the build process to fail. Up to now, the command is:

```Shell
$ make CC=gcc-12 CPP=g++-12 CXX=g++-12 LD=g++-12
```
