#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$ROOT/out/android-arm64/ast2-probe"

if [[ ! -f "$BIN" ]]; then
  echo "Build first with scripts/build-arm64.sh" >&2
  exit 1
fi

adb wait-for-device
adb push "$BIN" /data/local/tmp/ast2-probe >/dev/null
adb shell chmod 755 /data/local/tmp/ast2-probe

if [[ "${1:-}" == "--json" ]]; then
  adb shell /data/local/tmp/ast2-probe --json
else
  adb shell /data/local/tmp/ast2-probe
fi
