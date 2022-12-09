AUTOMAKE_OPTIONS=foreign
bin_PROGRAMS=AlicornCH
AM_CXXFLAGS=-I$(top_srcdir)/inc -O2 -Wall -Wno-unknown-pragmas -Wno-enum-compare -std=gnu++23
AM_CFLAGS=-I$(top_srcdir)/inc -O2 -Wall -Wno-unknown-pragmas -Wno-enum-compare -std=gnu17
AlicornCH_LDFLAGS=-L$(top_srcdir)/libs
AlicornCH_LDADD=-luv -lssl -lcrypto -lz

if WINDOWS
AlicornCH_LDADD+=-lws2_32 -lWebView2Loader.dll -lole32 -lshell32 -lshlwapi -luser32 -lcrypt32
endif

if LINUX
AM_CXXFLAGS+=$(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)
AM_CFLAGS+=$(pkg-config --cflags gtk+-3.0 webkit2gtk-4.0)
AlicornCH_LDFLAGS+=$(pkg-config --libs gtk+-3.0 webkit2gtk-4.0)
endif

if OSX
AM_CXXFLAGS+=-framework WebKit
AM_CFLAGS+=-framework WebKit
if ARM
# Flags for macOS ARM
AM_CXXFLAGS+=-I/opt/homebrew/include
AM_CFLAGS+=-I/opt/homebrew/include
AlicornCH_LDFLAGS+=-L/opt/homebrew/lib
else
# Flags for macOS x86
AM_CXXFLAGS+=-I/usr/local/include
AM_CFLAGS+=-I/usr/local/include
AlicornCH_LDFLAGS+=-L/usr/local/lib
endif
endif

AlicornCH_SOURCES=$(top_srcdir)/build/config.h $(top_srcdir)/inc/ach/core/network/Download.hh $(top_srcdir)/inc/ach/core/op/AssetsInstall.hh $(top_srcdir)/inc/ach/core/op/Finder.hh $(top_srcdir)/inc/ach/core/op/Flipper.hh $(top_srcdir)/inc/ach/core/op/Flow.hh $(top_srcdir)/inc/ach/core/op/GameLaunch.hh $(top_srcdir)/inc/ach/core/op/LibrariesInstall.hh $(top_srcdir)/inc/ach/core/op/NativesCheck.hh $(top_srcdir)/inc/ach/core/op/ProfileInstall.hh $(top_srcdir)/inc/ach/core/op/Tools.hh $(top_srcdir)/inc/ach/core/platform/OSType.hh $(top_srcdir)/inc/ach/core/profile/GameProfile.hh $(top_srcdir)/inc/ach/core/profile/Transform.hh $(top_srcdir)/inc/ach/core/runtime/GameInstance.hh $(top_srcdir)/inc/ach/drivers/aria2/Aria2Driver.hh $(top_srcdir)/inc/ach/sys/Init.hh $(top_srcdir)/inc/ach/sys/Options.hh $(top_srcdir)/inc/ach/sys/Schedule.hh $(top_srcdir)/inc/ach/util/Commons.hh $(top_srcdir)/inc/cJSON.h $(top_srcdir)/inc/httplib.h $(top_srcdir)/inc/lurl.hpp $(top_srcdir)/inc/miniz.h $(top_srcdir)/inc/uv/aix.h $(top_srcdir)/inc/uv/bsd.h $(top_srcdir)/inc/uv/darwin.h $(top_srcdir)/inc/uv/errno.h $(top_srcdir)/inc/uv/linux.h $(top_srcdir)/inc/uv/os390.h $(top_srcdir)/inc/uv/posix.h $(top_srcdir)/inc/uv/stdint-msvc2008.h $(top_srcdir)/inc/uv/sunos.h $(top_srcdir)/inc/uv/threadpool.h $(top_srcdir)/inc/uv/tree.h $(top_srcdir)/inc/uv/unix.h $(top_srcdir)/inc/uv/version.h $(top_srcdir)/inc/uv/win.h $(top_srcdir)/inc/uv.h $(top_srcdir)/inc/webview.h $(top_srcdir)/inc/WebView2.h $(top_srcdir)/src/core/network/Download.cc $(top_srcdir)/src/core/op/AssetsInstall.cc $(top_srcdir)/src/core/op/Finder.cc $(top_srcdir)/src/core/op/Flipper.cc $(top_srcdir)/src/core/op/Flow.cc $(top_srcdir)/src/core/op/GameLaunch.cc $(top_srcdir)/src/core/op/LibrariesInstall.cc $(top_srcdir)/src/core/op/NativesCheck.cc $(top_srcdir)/src/core/op/ProfileInstall.cc $(top_srcdir)/src/core/op/Tools.cc $(top_srcdir)/src/core/platform/OSType.cc $(top_srcdir)/src/core/profile/GameProfile.cc $(top_srcdir)/src/core/profile/Transform.cc $(top_srcdir)/src/core/runtime/GameInstance.cc $(top_srcdir)/src/drivers/aria2/Aria2Driver.cc $(top_srcdir)/src/libimpl/cJSON.c $(top_srcdir)/src/libimpl/httplib.cc $(top_srcdir)/src/libimpl/lurl.cc $(top_srcdir)/src/libimpl/miniz.c $(top_srcdir)/src/Main.cc $(top_srcdir)/src/sys/Init.cc $(top_srcdir)/src/sys/Options.cc $(top_srcdir)/src/sys/Schedule.cc $(top_srcdir)/src/util/Commons.cc
