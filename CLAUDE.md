# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A host-side (x86/Linux) mock of the Arduino/Teensy `SD` library. It lets Teensy
sketches and libraries that use `<SD.h>` be compiled and run on a desktop for
testing, backing SD card I/O with either real Linux files or an in-memory
buffer. It is part of the `teensy-x86-stubs` ecosystem and depends on that
sibling project for the Arduino runtime (`Arduino.h`, `Stream`, `Print`,
`initialize_mock_arduino()`).

## Build & test

The project builds with CMake and fetches dependencies at configure time via
`FetchContent` (network access required on first configure):
- `teensy-x86-stubs` (the Arduino core mock) — pulled by `DeclareAndFetch`.
- `cmake-declare-and-fetch` — the helper that provides `DeclareAndFetch`.

```bash
# Configure + build with tests (mirrors CI)
cmake -E make_directory build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=On
cmake --build .

# Run the full test suite (Boost.Test executable)
./test/tests

# Run a single test case by name
./test/tests --run_test=basic_write_tests/can_write_text

# Verbose / detailed output
./test/tests --log_level=all
```

Tests require Boost's unit test framework. On Ubuntu: `sudo apt-get install
libboost-test-dev`. Without `-DBUILD_TESTS=On` only the `teensy_x86_sd_stubs`
library is built (the `test/` subdir is skipped).

## Architecture

The public surface is `SDClass` (instantiated as the global `SD`) plus `File`,
declared in `src/SD.h` inside namespace `SDLib` (re-exported with `using
namespace SDLib` for sketch compatibility).

**Two backing modes, selected at runtime:**
- **In-memory** — `SD.setSDCardFileData(buf, size)` sets `_useMockData = true`.
  Every `SD.open(...)` then returns an `InMemoryFile` reading/writing that one
  buffer, regardless of filename. (`src/InMemoryFile.cpp`)
- **Folder-backed** — `SD.setSDCardFolderPath(path, createIfMissing)` maps a
  real Linux directory as the SD root. `SD.open(...)` returns a `LinuxFile`
  backed by `std::fstream` / `dirent`. (`src/LinuxFile.cpp`)

**Polymorphism via `AbstractFile`:** `File` (`src/File.cpp`) is a thin handle
that forwards every call to an owned `AbstractFile*`. `InMemoryFile` and
`LinuxFile` are the two concrete implementations. When adding file behavior,
add the virtual to `AbstractFile`, implement it in both subclasses, and forward
it from `File`.

**`SDClass` mutating ops shell out:** `mkdir`, `rmdir`, and `remove` build
`mkdir -p` / `rm -rf` command strings and call `system()` against the mapped
folder. `exists` probes via `std::fstream` and a `stat`-based directory check.

**`src/utility/` is vendored sdfatlib** (`Sd2Card`, `SdVolume`, `SdFile`,
`FatStructs.h`, etc.) carried over from the upstream Arduino SD library. The
real read/write path does **not** route through it — `SDClass::open` constructs
`InMemoryFile`/`LinuxFile` directly. The `walkPath`/`callback_*` path-traversal
machinery in `src/SD.cpp` still references `SdFile` but is largely legacy; the
`callback_openPath` callback is commented out. Prefer changing the
`InMemoryFile`/`LinuxFile`/`SDClass` layer over the `utility/` internals.

## Conventions

- C++14, library target is `teensy_x86_sd_stubs`.
- Tests link against `teensy_x86_sd_stubs`, `teensy_x86_stubs`, and Boost; each
  fixture calls `initialize_mock_arduino()` (see `test/default_test_fixture.h`).
- File-open modes use the Arduino `O_READ`/`O_WRITE` flags (`FILE_READ` /
  `FILE_WRITE` are convenience aliases defined in `SD.h`).
- CI (`.github/workflows/ubuntu-x86.yml`) configures with `BUILD_TESTS=On`,
  builds Release, runs `test/tests` emitting XML results.
