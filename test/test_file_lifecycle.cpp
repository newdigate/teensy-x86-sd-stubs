#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

#include <string>

// These tests exercise the File open/close lifecycle and File value-copy
// ownership semantics introduced when File began owning its AbstractFile impl
// via std::shared_ptr (issue #10). They guard against use-after-free and
// double-free regressions.
//
// NOTE: true memory-leak detection requires a sanitizer build
// (e.g. -fsanitize=address) which is out of scope here. These cases verify
// the lifecycle/ownership *behavior* does not crash and produces correct reads.

BOOST_AUTO_TEST_SUITE(file_lifecycle_tests)

    // Open (write then read) a file many times to exercise the
    // new/shared_ptr/close path repeatedly without crashing.
    BOOST_FIXTURE_TEST_CASE(repeated_open_close_does_not_crash, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);

        const char *expected = "hello world";
        const int iterations = 100;

        for (int i = 0; i < iterations; i++) {
            if (SD.exists("lifecycle.txt")) {
                SD.remove("lifecycle.txt");
            }

            File write_file = SD.open("lifecycle.txt", O_WRITE);
            if (!write_file) {
                BOOST_FAIL("File was not opened (for write)...");
            }
            write_file.write((const uint8_t *)expected, 11);
            write_file.close();

            File read_file = SD.open("lifecycle.txt", O_READ);
            if (!read_file) {
                BOOST_FAIL("File was not opened (for read)...");
            }
            char buffer[11] = {0};
            int numBytesRead = read_file.read(buffer, 11);
            BOOST_CHECK_EQUAL(numBytesRead, 11);
            BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[10],
                                          &expected[0], &expected[10]);
            read_file.close();
        }

        if (SD.exists("lifecycle.txt")) {
            SD.remove("lifecycle.txt");
        }
    }

    // Copying a File shares ownership of the underlying impl. Closing one copy
    // must not leave the other dangling, and the impl must be freed exactly
    // once (no double-free) when the last copy goes away.
    BOOST_FIXTURE_TEST_CASE(copying_file_then_closing_is_safe, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);

        const char *expected = "hello world";

        if (SD.exists("copy.txt")) {
            SD.remove("copy.txt");
        }

        File write_file = SD.open("copy.txt", O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((const uint8_t *)expected, 11);
        write_file.close();

        {
            File a = SD.open("copy.txt", O_READ);
            if (!a) {
                BOOST_FAIL("File was not opened (for read)...");
            }
            File b = a;   // shares ownership of the impl

            char buffer[11] = {0};
            int numBytesRead = b.read(buffer, 11);
            BOOST_CHECK_EQUAL(numBytesRead, 11);

            // Close through one handle; the other still refers to the (now
            // closed) shared impl, which is freed when the last copy drops.
            a.close();
            // b goes out of scope here without an explicit close().
        }

        if (SD.exists("copy.txt")) {
            SD.remove("copy.txt");
        }
    }

BOOST_AUTO_TEST_SUITE_END()
