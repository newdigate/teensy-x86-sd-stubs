#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

BOOST_AUTO_TEST_SUITE(linuxfile_write_size_tests)

    // Regression for issue #7: writing in place after a seek must not inflate
    // size(). The key assertion is on the live (still-open) file handle, since
    // reopening recomputes size from disk and would mask the bug.
    BOOST_FIXTURE_TEST_CASE(seek_overwrite_does_not_inflate_size, DefaultTestFixture) {

        const char *expected = "hello universe";

        SD.setSDCardFolderPath("output", true);

        if (SD.exists("test_write_size.txt")) {
            SD.remove("test_write_size.txt");
        }

        File write_file = SD.open("test_write_size.txt", O_READ | O_WRITE | O_TRUNC);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }

        write_file.write((const uint8_t *)"hello world", 11);
        BOOST_CHECK_EQUAL(write_file.size(), 11);

        write_file.seek(6);
        write_file.write((const uint8_t *)"universe", 8);

        // seek(6) + write(8) on an 11-byte file ends at position 14, so the file
        // is 14 bytes -- not 11 + 8 == 19. Assert on the live handle.
        BOOST_CHECK_EQUAL(write_file.size(), 14);

        write_file.close();

        File read_file = SD.open("test_write_size.txt", O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }
        BOOST_CHECK_EQUAL(read_file.size(), 14);

        char buffer[14];
        int numBytesRead = read_file.read(buffer, 20);
        BOOST_CHECK_EQUAL(numBytesRead, 14);
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[13], &expected[0], &expected[13]);
        read_file.close();
    }

BOOST_AUTO_TEST_SUITE_END()
