#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

#include <cstdio>
#include <fstream>

BOOST_AUTO_TEST_SUITE(linuxfile_truncate_tests)

    // Regression for issue #6: LinuxFile::truncate must operate on the file
    // under the SD folder (localFileName), not on the bare filename relative
    // to the process CWD. Before the fix, ::truncate received the bare name,
    // so the file under the SD folder was left unchanged.
    BOOST_FIXTURE_TEST_CASE(truncate_targets_file_under_sd_folder, DefaultTestFixture) {

        SD.setSDCardFolderPath("output", true);

        const char *fileName = "truncate_me.txt";

        if (SD.exists(fileName)) {
            SD.remove(fileName);
        }
        // Make sure no stray file from a previous run sits in the CWD.
        std::remove(fileName);

        // Arrange: write known content to the file under the SD folder.
        File write_file = SD.open(fileName, O_WRITE);
        if (!write_file) {
            BOOST_FAIL("File was not opened (for write)...");
        }
        write_file.write((const uint8_t *)"hello world", 11);
        write_file.close();

        // Act: open the file and truncate it.
        File trunc_file = SD.open(fileName, O_READ | O_WRITE);
        if (!trunc_file) {
            BOOST_FAIL("File was not opened (for truncate)...");
        }
        BOOST_CHECK(trunc_file.truncate(0));
        trunc_file.close();

        // Assert: the file UNDER THE SD FOLDER is now empty. Reopening via SD
        // recomputes the size from disk, so size() reflects the truncation.
        File read_file = SD.open(fileName, O_READ);
        if (!read_file) {
            BOOST_FAIL("File was not opened (for read)...");
        }
        BOOST_CHECK_EQUAL(read_file.size(), 0u);
        read_file.close();

        // And confirm directly on disk that the SD-folder file shrank.
        std::ifstream onDisk("output/truncate_me.txt", std::ios::binary | std::ios::ate);
        BOOST_REQUIRE(onDisk.is_open());
        BOOST_CHECK_EQUAL((long)onDisk.tellg(), 0L);
        onDisk.close();

        // And confirm no stray file was created in the process CWD (the old
        // buggy behaviour would have aimed ::truncate at "truncate_me.txt"
        // in the CWD).
        std::ifstream stray("truncate_me.txt", std::ios::binary);
        BOOST_CHECK(!stray.is_open());
        if (stray.is_open()) stray.close();
    }

BOOST_AUTO_TEST_SUITE_END()
