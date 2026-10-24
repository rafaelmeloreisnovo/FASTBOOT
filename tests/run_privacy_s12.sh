#!/bin/sh
# Host-only privacy harness: there is NO physical USB IO and NO L0 OS dependency.
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
WARN='-Wall -Wextra -Werror -Wshadow -Wconversion -Wstrict-prototypes -Wmissing-prototypes'
"$CC" -std=gnu11 -O2 $WARN -Iinclude -include tests/fb_mock_prototypes.h \
  -Dmain=fb_linux_usbfs_main -c host/linux_usbfs.c -o build/s12_host.o
"$CC" -std=gnu11 -O2 $WARN -Iinclude \
  -c tests/test_usb_host_privacy_s12.c -o build/s12_mock.o
"$CC" -std=gnu11 -O2 $WARN -Iinclude \
  src/fb_omega.c src/fb_guard.c src/fb_usb_desc.c \
  build/s12_host.o build/s12_mock.o \
  -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=clock_gettime -Wl,--wrap=ioctl \
  -o build/s12_privacy_mock
./build/s12_privacy_mock
"$CLANG" -std=gnu11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  $WARN -Iinclude -include tests/fb_mock_prototypes.h -Dmain=fb_linux_usbfs_main \
  -c host/linux_usbfs.c -o build/s12_host_san.o
"$CLANG" -std=gnu11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  $WARN -Iinclude -c tests/test_usb_host_privacy_s12.c -o build/s12_mock_san.o
"$CLANG" -std=gnu11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
  $WARN -Iinclude src/fb_omega.c src/fb_guard.c src/fb_usb_desc.c \
  build/s12_host_san.o build/s12_mock_san.o \
  -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=clock_gettime -Wl,--wrap=ioctl \
  -o build/s12_privacy_mock_san
ASAN_OPTIONS=detect_leaks=1 ./build/s12_privacy_mock_san
printf '%s\n' 'PASS_SCOPED: S12 privacy-by-default and explicit opt-in; host USB fully mocked; no real USB'
