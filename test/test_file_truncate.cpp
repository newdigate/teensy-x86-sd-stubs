#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

BOOST_AUTO_TEST_SUITE(file_truncate_tests)

    BOOST_FIXTURE_TEST_CASE(truncate_honors_size_argument, DefaultTestFixture) {

        SD.setSDCardFolderPath("output", true);

        if (SD.exists("truncate.txt")) {
            SD.remove("truncate.txt");
        }

        // Write a known 11-byte string.
        File write_file = SD.open("truncate.txt", O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((const uint8_t *)"hello world", 11);
        write_file.close();

        // Reopen and truncate to 5 bytes.
        File trunc_file = SD.open("truncate.txt", O_READ | O_WRITE);
        if (!trunc_file) {
            BOOST_FAIL("File was not opened (for truncate)...");
        }
        BOOST_CHECK(trunc_file.truncate(5));
        trunc_file.close();

        // Reopen for read and verify the file is exactly 5 bytes ("hello").
        File read_file = SD.open("truncate.txt", O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }
        BOOST_CHECK_EQUAL(read_file.size(), 5u);

        const char *expected = "hello";
        char buffer[16] = {0};
        int numBytesRead = read_file.read(buffer, 16);
        read_file.close();

        BOOST_CHECK_EQUAL(numBytesRead, 5);
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[5], &expected[0], &expected[5]);
    }

BOOST_AUTO_TEST_SUITE_END()
