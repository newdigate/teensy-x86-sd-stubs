# Test strategy

This project uses [Boost.Test](https://www.boost.org/doc/libs/release/libs/test/)
for unit tests. All test files are compiled into a single executable
(`test/tests`) which CI runs on every push.

## Layout & conventions

- **One file per area**, named `test_<area>.cpp` (e.g. `test_truncate.cpp`,
  `test_read.cpp`). `test/CMakeLists.txt` globs `test_*.cpp`, so a new file is
  compiled automatically — no CMake edits needed.
- **Exactly one file defines the Boost test module.** `test_write.cpp` owns
  `#define BOOST_TEST_MODULE ArduinoSDEmulationTests`. **No other test file may
  define `BOOST_TEST_MODULE`** — they only `#include <boost/test/unit_test.hpp>`
  and declare suites. (Two definitions break the link with duplicate symbols.)
- **Wrap cases in a uniquely named suite:**
  `BOOST_AUTO_TEST_SUITE(<area>_tests)` … `BOOST_AUTO_TEST_SUITE_END()`.
- **Use the shared fixture** so the Arduino mock is initialized:
  `BOOST_FIXTURE_TEST_CASE(case_name, DefaultTestFixture)` (from
  `default_test_fixture.h`, which calls `initialize_mock_arduino()`).
- **File-backed tests** should map a working directory via
  `SD.setSDCardFolderPath("output", true)` and clean up artifacts they create
  (the existing tests write under `output/`).

## Minimal template

```cpp
#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

BOOST_AUTO_TEST_SUITE(example_tests)

    BOOST_FIXTURE_TEST_CASE(does_the_thing, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);
        // ... arrange / act / assert with BOOST_CHECK_* ...
    }

BOOST_AUTO_TEST_SUITE_END()
```

## Running

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=On
cmake --build build
./build/test/tests                                   # all tests
./build/test/tests --run_test=example_tests/does_the_thing   # one case
./build/test/tests --log_level=all                   # verbose
```

CI (`.github/workflows/ubuntu-x86.yml`) configures with `-DBUILD_TESTS=On`,
builds, and runs `test/tests` emitting XML results — so any `test_*.cpp` added
here runs in CI automatically.
