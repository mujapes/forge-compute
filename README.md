# Public CPU build harness

A bounded public GitHub Actions job builds a generic [Animation Compression
Library](https://github.com/nfrechette/acl) sampler for WebAssembly and compares
1,028 synthetic time/rounding samples against the native library build.

Only the small generic sampling wrapper, build scripts and synthetic test
construction are public. There are no private repository checkouts, secrets,
game assets, original executable bytes or private project files. Dependencies
are pinned by full Git commit. Jobs use a standard Ubuntu runner, a 12-minute
limit and one-day artifacts; no schedules, model calls or paid runners.

The caller supplies default transforms in compressed-track order explicitly.
Output is contiguous float32 xyzw rotation, xyz translation and xyz scale.
This is an open-source codec implementation, not evidence of any game's engine
sampling, default selection, retargeting, animation graph or live parity.

Run `bash fetch-sources.sh && bash build.sh` with g++, Emscripten and Node installed.
The workflow installs compiler dependencies on GitHub, not on the phone.
