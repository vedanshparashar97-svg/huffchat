#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ ! -x ./huffchat_cli || ./huffman_cli.cpp -nt ./huffchat_cli ]]; then ./build.sh; fi
exec python3 server.py
