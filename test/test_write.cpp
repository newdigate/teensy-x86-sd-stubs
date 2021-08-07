#define BOOST_TEST_MODULE ArduinoSDEmulationTests
#include <boost/test/unit_test.hpp>
#include "default_test_fixture.h"

#include <iostream>
#include <fstream>
BOOST_AUTO_TEST_SUITE(basic_write_tests)

    BOOST_FIXTURE_TEST_CASE(can_write_text, DefaultTestFixture) {

        const char *expected = "hello world";

        system("echo ${PWD}");
        SD.setSDCardFolderPath("output", true);

        if (SD.exists("test.txt")) {
            SD.remove("test.txt");
        }

        File write_file = SD.open("test.txt", O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((const uint8_t *)"hello world", 11);
        write_file.close();


        File read_file = SD.open("test.txt", O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }
        char buffer[11];
        int numBytesRead = read_file.read(buffer, 11);
        BOOST_CHECK_EQUAL(numBytesRead, 11);
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[10], &expected[0], &expected[10]);
    }

    BOOST_FIXTURE_TEST_CASE(can_seek_write_text, DefaultTestFixture) {

        const char *expected = "hello universe";

        system("echo ${PWD}");
        SD.setSDCardFolderPath("output", true);

        if (SD.exists("test.txt")) {
            SD.remove("test.txt");
        }

        File write_file = SD.open("test.txt", O_WRITE | O_APPEND);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((const uint8_t *)"hello world", 11);
        write_file.close();


        File write_file2 = SD.open("test.txt", O_READ | O_WRITE);
        if (!write_file2) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file2.seek(6);
        write_file2.write((const uint8_t *)"universe", 8);
        write_file2.close();


        File read_file = SD.open("test.txt", O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }
        char buffer[14];
        int numBytesRead = read_file.read(buffer, 20);
        BOOST_CHECK_EQUAL(numBytesRead, 14);
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[13], &expected[0], &expected[13]);
    }

    BOOST_FIXTURE_TEST_CASE(lowlevel_seek_write, DefaultTestFixture) {

        const char *expected = "helloxxxrld";

        std::fstream f1;
        f1.open("output/test.txt", std::fstream::out);
        if (!f1) {
            BOOST_FAIL("File was not opened (for write)...");
        }

        f1.write("hello world", 11);
        f1.seekp(5);
        f1.write("xxx", 3);
        f1.close();

        std::fstream f3;
        f3.open("output/test.txt", std::fstream::in);
        char buffer[11] = {0};
        f3.read(buffer, 11);
        f3.close();
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[10], &expected[0], &expected[10]);
    }

    BOOST_FIXTURE_TEST_CASE(lowlevel_seek_write_2, DefaultTestFixture) {

        const char *expected = "helloxxxrld";
        std::fstream f1;
        f1.open("output/test.txt", std::fstream::out);
        if (!f1) {
            BOOST_FAIL("File was not opened (for write)...");
        }

        f1.write("hello world", 11);
        f1.close();

        std::fstream f2;
        f2.open("output/test.txt", std::fstream::in | std::fstream::out);
        f2.seekp(5);
        f2.write("xxx", 3);
        f2.close();

        std::fstream f3;
        f3.open("output/test.txt", std::fstream::in);
        char buffer[11] = {0};
        f3.read(buffer, 11);
        f3.close();
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[10], &expected[0], &expected[10]);

    }

BOOST_AUTO_TEST_SUITE_END()