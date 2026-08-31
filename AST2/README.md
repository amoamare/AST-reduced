# AST2 modernization

This directory is the incremental successor to the historical AST implementation under `jni/`.

The legacy source remains untouched. AST2 starts with a small, testable native core and ports functionality only after its transport and authorization requirements are understood.

## Phase 1 goals

- Android NDK + CMake build; no checked-in build products.
- C++20, RAII, explicit ownership, no implementation-file includes.
- Runtime Binder capability discovery rather than Android/model version branches.
- Typed capability/result reporting rather than `OK`/`FAIL` text parsing.
- Samsung framework Binder discovery (`phone`, `isemtelephony`) without issuing modem commands.
- Runtime `libbinder_ndk.so` loading so platform ServiceManager symbols can be used across NDK revisions without linking private platform libraries.
- Separate research/discovery code from service operations.

## Current scope

`ast2-probe` is observational. It obtains Binder handles and pings known services. It does **not** send OEM-hook, AT, NV, unlock, reset, or other modem payloads.

## Build

```bash
export ANDROID_NDK_HOME="$HOME/Library/Android/sdk/ndk/<version>"
cmake -S AST2 -B AST2/out/android-arm64 -G Ninja \
  -DCMAKE_TOOLCHAIN_FILE="$ANDROID_NDK_HOME/build/cmake/android.toolchain.cmake" \
  -DANDROID_ABI=arm64-v8a \
  -DANDROID_PLATFORM=android-31 \
  -DCMAKE_BUILD_TYPE=Release
cmake --build AST2/out/android-arm64
```

Push and run:

```bash
adb push AST2/out/android-arm64/ast2-probe /data/local/tmp/ast2-probe
adb shell chmod 755 /data/local/tmp/ast2-probe
adb shell /data/local/tmp/ast2-probe
```

## Direction

The intended architecture is:

```text
Desktop control plane
        |
        v
  AST2 device agent
        |
        +-- Capability discovery
        +-- Transport resolver
        |      +-- Framework Binder
        |      +-- Vendor AIDL/HIDL (when legitimately accessible)
        |      +-- Vendor native interfaces (authorized environments)
        |
        +-- Vendor capability layer
               +-- Samsung
               +-- Qualcomm
               +-- Motorola
```

Operations will depend on capabilities, not hard-coded model/version branches.
