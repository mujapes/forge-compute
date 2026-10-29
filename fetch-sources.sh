#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p vendor
fetch() {
 local source_url="$1" source_ref="$2" source_dir="$3"
 git init -q "$source_dir"
 git -C "$source_dir" remote add origin "$source_url"
 git -C "$source_dir" fetch -q --depth 1 origin "$source_ref"
 git -C "$source_dir" checkout -q --detach FETCH_HEAD
 test "$(git -C "$source_dir" rev-parse HEAD)" = "$source_ref"
}
fetch https://github.com/nfrechette/acl.git 414689d5cff4286a7898487a46dc5e48005d38da vendor/acl
fetch https://github.com/nfrechette/rtm.git d7982f2b2524feeb322f424b29cf43df30b5d5b7 vendor/rtm
