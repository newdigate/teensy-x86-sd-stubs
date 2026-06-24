#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

BOOST_AUTO_TEST_SUITE(linuxfile_read_eof_tests)

    BOOST_FIXTURE_TEST_CASE(read_returns_minus_one_at_eof, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);

        if (SD.exists("eof.txt")) {
            SD.remove("eof.txt");
        }

        File write_file = SD.open("eof.txt", O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((const uint8_t *)"abc", 3);
        write_file.close();

        File read_file = SD.open("eof.txt", O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }

        BOOST_CHECK_EQUAL(read_file.read(), (int)'a');
        BOOST_CHECK_EQUAL(read_file.read(), (int)'b');
        BOOST_CHECK_EQUAL(read_file.read(), (int)'c');

        // Past the end of the file: read() must signal EOF with -1.
        BOOST_CHECK_EQUAL(read_file.read(), -1);

        read_file.close();
    }

    BOOST_FIXTURE_TEST_CASE(read_high_byte_is_not_sign_extended, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);

        if (SD.exists("highbyte.txt")) {
            SD.remove("highbyte.txt");
        }

        File write_file = SD.open("highbyte.txt", O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((uint8_t)0xFE);
        write_file.close();

        File read_file = SD.open("highbyte.txt", O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }

        // A byte >= 0x80 must come back as a positive value, not sign-extended.
        BOOST_CHECK_EQUAL(read_file.read(), 254);

        read_file.close();
    }

BOOST_AUTO_TEST_SUITE_END()
