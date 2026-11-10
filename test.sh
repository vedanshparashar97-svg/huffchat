#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
./build.sh >/dev/null
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
check_case(){ local label="$1"; local input="$2"; printf '%s' "$input" > "$TMP/input"; ./huffchat_cli compress "$TMP/input" "$TMP/data.huff" >/dev/null; ./huffchat_cli decompress "$TMP/data.huff" "$TMP/output" >/dev/null; cmp "$TMP/input" "$TMP/output"; echo "PASS: $label"; }
check_case "ordinary text" 'hello world, HuffChat!'
check_case "single repeated byte" 'aaaaaaaaaaaaaaaaaaaaaaaaaaaa'
check_case "empty input" ''
check_case "unicode UTF-8" 'नमस्ते दुनिया — HuffChat 🚀'
python3 - <<PY
from pathlib import Path
Path('$TMP/input').write_bytes(bytes(range(256))*100)
PY
./huffchat_cli compress "$TMP/input" "$TMP/data.huff" >/dev/null
./huffchat_cli decompress "$TMP/data.huff" "$TMP/output" >/dev/null
cmp "$TMP/input" "$TMP/output" && echo 'PASS: all byte values / binary round trip'
cp "$TMP/data.huff" "$TMP/bad.huff"
printf '\001' | dd of="$TMP/bad.huff" bs=1 seek=0 conv=notrunc status=none
if ./huffchat_cli decompress "$TMP/bad.huff" "$TMP/bad.out" >/dev/null 2>&1; then echo 'FAIL: corrupted archive accepted'; exit 1; else echo 'PASS: corrupted archive rejected'; fi
