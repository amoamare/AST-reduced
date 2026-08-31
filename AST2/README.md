# AST2 modernization

This directory is the incremental successor to the historical AST implementation under `jni/`.

The legacy source remains untouched. AST2 starts with a small, testable native core and ports functionality only after its transport and authorization requirements are understood.

## Current foundation

AST2 now includes:

- Android NDK + CMake C++20 build.
- RAII-style runtime `libbinder_ndk.so` loading.
- `DeviceProfile` detection for model, Android version, security patch, chipset/platform and UID.
- typed `Capability` reporting.
- generalized framework Binder service probing.
- Samsung Binder probing for `phone` and `isemtelephony`.
- `IModemTransport` as the common transport capability boundary.
- `SamsungTransactionProfile` for firmware-derived Binder transaction metadata.
- initial `IOperation` state/result model for future service operations.
- human-readable and `--json` probe output.

The current executable remains observational. It obtains Binder handles and pings services but does **not** send OEM-hook, AT, NV, unlock, reset, or other modem payloads.

## Build

```bash
cd AST2
export ANDROID_NDK_HOME="$HOME/Library/Android/sdk/ndk/<version>"
./scripts/build-arm64.sh
```

Run:

```bash
./scripts/run-probe.sh
```

Structured output:

```bash
./scripts/run-probe.sh --json
```

## Architecture

```text
Desktop control plane
        |
        v
  AST2 device agent
        |
        +-- DeviceProfile
        +-- Capability discovery
        +-- Transport resolver
        |      +-- Framework Binder
        |      +-- Vendor AIDL/HIDL
        |      +-- Vendor native interfaces
        |
        +-- Vendor layer
        |      +-- Samsung transaction profiles
        |      +-- Qualcomm profiles
        |      +-- Motorola profiles
        |
        +-- Operation engine
               Created
                 -> Validating
                 -> OpeningTransport
                 -> Executing
                 -> Verifying
                 -> Completed / Failed
```

## Firmware profiles

Transaction numbers and descriptors belong in firmware/device profiles rather than operation code.

The first recorded profile is the research SM-N975U Android 12 firmware:

```text
service: isemtelephony
descriptor: com.android.internal.telephony.ISemTelephony
sendRequestToRIL: 14
invokeOemRilRequestRawForSubscriber: 28
```

This metadata is intentionally separate from execution logic so future profiles can be derived from their matching framework artifacts without introducing model/version conditionals throughout the codebase.

## Next milestones

1. transport registry/resolver;
2. generalized service catalog and capability aggregation;
3. profile matching by firmware/build fingerprint;
4. typed protocol codecs replacing magic byte arrays;
5. operation runner with timeout/retry/verification policies;
6. desktop RPC framing and structured event/log output.

The historical AST code remains available as a reference while functionality is ported incrementally.
