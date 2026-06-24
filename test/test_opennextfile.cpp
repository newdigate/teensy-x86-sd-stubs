#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

#include <string>
#include <vector>
#include <algorithm>

BOOST_AUTO_TEST_SUITE(opennextfile_tests)

    BOOST_FIXTURE_TEST_CASE(skips_dot_and_dotdot_and_lists_children, DefaultTestFixture) {
        SD.setSDCardFolderPath("output", true);

        const char *subdir = "onf_dir";
        const char *fileA = "onf_dir/alpha.txt";
        const char *fileB = "onf_dir/beta.txt";

        // Clean any leftovers from a previous run.
        SD.remove(fileA);
        SD.remove(fileB);
        SD.rmdir(subdir);

        // Arrange: create a subdirectory with two known files.
        BOOST_REQUIRE(SD.mkdir(subdir));

        File wa = SD.open(fileA, O_WRITE);
        // File::operator bool() is non-const, so convert explicitly before
        // handing the result to a Boost assertion (which takes a const ref).
        if (!wa) BOOST_FAIL("could not open alpha.txt for write");
        wa.write((const uint8_t *)"a", 1);
        wa.close();

        File wb = SD.open(fileB, O_WRITE);
        if (!wb) BOOST_FAIL("could not open beta.txt for write");
        wb.write((const uint8_t *)"b", 1);
        wb.close();

        // Act: iterate the directory collecting child names.
        File dir = SD.open(subdir, O_READ);
        if (!dir) BOOST_FAIL("could not open onf_dir for read");
        BOOST_REQUIRE(dir.isDirectory());

        std::vector<std::string> names;
        while (true) {
            File child = dir.openNextFile();
            if (!child)
                break;
            names.push_back(std::string(child.name()));
            child.close();
        }
        dir.close();

        // Assert: "." and ".." pseudo-entries are not returned.
        BOOST_CHECK(std::find(names.begin(), names.end(), std::string(".")) == names.end());
        BOOST_CHECK(std::find(names.begin(), names.end(), std::string("..")) == names.end());

        // Assert: the known files are present.
        BOOST_CHECK(std::find(names.begin(), names.end(), std::string("alpha.txt")) != names.end());
        BOOST_CHECK(std::find(names.begin(), names.end(), std::string("beta.txt")) != names.end());

        // Clean up artifacts.
        SD.remove(fileA);
        SD.remove(fileB);
        SD.rmdir(subdir);
    }

BOOST_AUTO_TEST_SUITE_END()
