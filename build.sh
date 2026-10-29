#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p dist
flags=(-O3 -std=c++11 -I vendor/acl/includes -I vendor/rtm/includes)
g++ "${flags[@]}" src/acl_sample.cpp tests/generate.cpp -o dist/generate
dist/generate
em++ "${flags[@]}" src/acl_sample.cpp --no-entry -s MODULARIZE=1 -s EXPORT_ES6=1 -s ENVIRONMENT=web,node -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=134217728 -s EXPORTED_FUNCTIONS='["_acl_validate","_acl_sample","_malloc","_free"]' -s EXPORTED_RUNTIME_METHODS='["HEAPU8","HEAPF32"]' -o dist/acl-sample.mjs
node tests/wasm.mjs
sha256sum dist/acl-sample.mjs dist/acl-sample.wasm > dist/SHA256SUMS
