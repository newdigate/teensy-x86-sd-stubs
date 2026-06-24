#include <boost/test/unit_test.hpp>   // do NOT define BOOST_TEST_MODULE here
#include "default_test_fixture.h"

BOOST_AUTO_TEST_SUITE(sdclass_init_tests)

    // Determinism check for issue #9: the default constructor used to leave
    // _useMockData, _fileData and _fileSize indeterminate, yet begin() reads
    // them. With the in-class member initializers a freshly constructed SDClass
    // has no folder and no mock data, so begin() must deterministically return
    // false before any setter is called. (Before the fix the members were
    // indeterminate and begin() could return either value.)
    BOOST_FIXTURE_TEST_CASE(fresh_sdclass_begin_is_false, DefaultTestFixture) {
        SDClass localSd;
        BOOST_CHECK_EQUAL(localSd.begin(), false);
    }

BOOST_AUTO_TEST_SUITE_END()
