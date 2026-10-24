# FASTBOOT Ω — RAFAELIA

Portable **read-only Fastboot L0**: one freestanding C11 parser shared across notebook→phone and future Android phone→phone USB host adapters. Experimental source-only; not a firmware flasher or unlock tool.

## Quick test (Linux / clang or gcc)

~~~sh
mkdir -p build
cc -std=c11 -O2 -Wall -Wextra -Werror -Iinclude src/fb_omega.c tests/test_fb.c -o build/test_fb
./build/test_fb
cc -std=gnu11 -O2 -Wall -Wextra -Werror -Iinclude src/fb_omega.c host/linux_usbfs.c -o build/fb-linux
cc -std=c11 -O2 -ffreestanding -fno-builtin -nostdlib -Iinclude -c src/fb_omega.c -o build/fb_l0.o
nm -u build/fb_l0.o
~~~

The L0 object must have **no undefined external symbols**. The Linux host executable is **not** freestanding: USB permissions and OS syscalls belong to its explicit adapter. See [architecture](docs/ARCHITECTURE.md). Do not operate on connected devices before checking exact USB interface and endpoint descriptors.

### Governed status
SOURCE_IMPLEMENTED_L0 | LOCAL_SYNTHETIC_PASS_56 | ANDROID_HOST_SOURCE_ONLY | PHYSICAL_NOT_RUN | claim_allowed=false | NO_MERGE.

This work preserves the initial main state as predecessor, adds no flash/unlock/erase/reboot paths, and does not claim origin of the publicly documented Fastboot protocol.
