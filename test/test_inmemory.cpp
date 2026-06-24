#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

BOOST_AUTO_TEST_SUITE(inmemory_tests)

    BOOST_FIXTURE_TEST_CASE(can_read_inmemory_buffer, DefaultTestFixture) {
        const char *expected = "abcdef";

        SD.setSDCardFileData((char*)"abcdef", 6);

        // In in-memory mode any filename returns the one backing buffer.
        File f = SD.open("anything");
        if (!f) {
            BOOST_FAIL("In-memory file was not opened...");
        }

        BOOST_CHECK(f.available());

        char buffer[6] = {0};
        int numBytesRead = f.read(buffer, 6);
        BOOST_CHECK_EQUAL(numBytesRead, 6);
        BOOST_CHECK_EQUAL_COLLECTIONS(&buffer[0], &buffer[6], &expected[0], &expected[6]);

        // After reading the whole buffer, available() must report false
        // (exercises the && / signed-unsigned guard in available()).
        BOOST_CHECK(!f.available());

        f.close();
    }

BOOST_AUTO_TEST_SUITE_END()
