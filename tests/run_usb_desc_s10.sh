#!/bin/sh
# S10 host tests use libc; only src/fb_usb_desc.c is freestanding L0.
set -eu
mkdir -p build
CC=${CC:-cc}
CLANG=${CLANG:-clang}
"$CC" -std=c11 -O2 -Wall -Wextra -Werror -Wshadow -Wconversion \
 -Iinclude src/fb_usb_desc.c tests/test_usb_desc_s10.c -o build/test_usb_desc_s10
./build/test_usb_desc_s10
"$CLANG" -std=c11 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer \
 -Wall -Wextra -Werror -Iinclude src/fb_usb_desc.c tests/test_usb_desc_s10.c \
 -o build/test_usb_desc_s10_sanitized
ASAN_OPTIONS=detect_leaks=1 ./build/test_usb_desc_s10_sanitized
"$CC" -std=gnu11 -O2 -Wall -Wextra -Werror -Wshadow -Wconversion \
 -Wstrict-prototypes -Wmissing-prototypes -Iinclude \
 -include tests/fb_mock_prototypes.h -Dmain=fb_linux_usbfs_main \
 -c host/linux_usbfs.c -o build/fb_linux_s10_main.o
"$CC" -std=gnu11 -O2 -Wall -Wextra -Werror -Wshadow -Wconversion \
 -Wstrict-prototypes -Wmissing-prototypes -Iinclude \
 -c tests/test_usb_host_mock_s10.c -o build/fb_linux_s10_mock.o
"$CC" -std=gnu11 -O2 -Wall -Wextra -Werror -Wshadow -Wconversion \
 -Iinclude src/fb_omega.c src/fb_guard.c src/fb_usb_desc.c \
 build/fb_linux_s10_main.o build/fb_linux_s10_mock.o \
 -Wl,--wrap=open -Wl,--wrap=close -Wl,--wrap=clock_gettime -Wl,--wrap=ioctl \
 -o build/test_usb_host_mock_s10
./build/test_usb_host_mock_s10
"$CLANG" -std=c11 -O2 -target armv7a-none-eabi -ffreestanding \
 -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -Iinclude \
 -c src/fb_usb_desc.c -o build/fb_usb_desc_s10_armv7_original.o
"$CLANG" -std=c11 -O2 -target aarch64-none-elf -ffreestanding \
 -fno-builtin -fno-stack-protector -Wall -Wextra -Werror -Iinclude \
 -c src/fb_usb_desc.c -o build/fb_usb_desc_s10_aarch64_original.o
for arch in armv7 aarch64; do
  original="build/fb_usb_desc_s10_${arch}_original.o"
  test -z "$(nm -u "$original")" || { echo "FAIL L0 external $arch" >&2; exit 1; }
  # ARMv7 EHABI .exidx relocation is retained in original and excluded
  # from the separately derived instruction-only object.
  derivative="build/fb_usb_desc_s10_${arch}_textonly.o"
  if [ "$arch" = armv7 ]; then
    llvm-objcopy --remove-section=.ARM.exidx --remove-section=.rel.ARM.exidx \
      "$original" "$derivative"
  else
    cp "$original" "$derivative"
  fi
  readelf -r "$derivative" | grep -q 'There are no relocations in this file' || {
    echo "FAIL: derivative relocations $arch" >&2;exit 1;
  }
  llvm-objcopy -O binary -j .text "$derivative" "build/fb_usb_desc_s10_${arch}.bin"
  test -s "build/fb_usb_desc_s10_${arch}.bin" || exit 1
done
echo 'PASS S10: freestanding parser, mocks, sanitizers, ARM objects; physical USB NOT_RUN'
