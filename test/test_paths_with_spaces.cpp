#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

// Regression test for issue #12: SD.mkdir/rmdir/remove used to shell out via
// system() with unquoted paths, which breaks on paths containing spaces (and
// is a command-injection risk). With the std::filesystem implementation these
// operations work correctly regardless of spaces in the path.
BOOST_AUTO_TEST_SUITE(paths_with_spaces_tests)

    BOOST_FIXTURE_TEST_CASE(mkdir_remove_handle_spaces, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);

        const char *dirName = "a folder";
        const char *filePath = "a folder/my file.txt";

        // Clean up any artifacts left over from a previous run.
        if (SD.exists(dirName)) {
            SD.rmdir(dirName);
        }
        BOOST_CHECK(!SD.exists(dirName));

        // Create a directory whose name contains a space.
        BOOST_CHECK(SD.mkdir(dirName));
        BOOST_CHECK(SD.exists(dirName));

        // Write a file (with a space in its name) inside that directory.
        File write_file = SD.open(filePath, O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File with a space in its path was not opened (for write)...");
        }
        write_file.write((const uint8_t *)"hello world", 11);
        write_file.close();
        BOOST_CHECK(SD.exists(filePath));

        // remove() should delete the file (recursively handles dirs/files).
        SD.remove(filePath);
        BOOST_CHECK(!SD.exists(filePath));

        // rmdir() should delete the directory itself.
        SD.rmdir(dirName);
        BOOST_CHECK(!SD.exists(dirName));
    }

BOOST_AUTO_TEST_SUITE_END()
