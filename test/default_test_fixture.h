#ifndef TEENSY_X86_SD_STUBS_DEFAULT_TEST_FIXTURE_H
#define TEENSY_X86_SD_STUBS_DEFAULT_TEST_FIXTURE_H

#include <Arduino.h>
#include <SD.h>

struct DefaultTestFixture
{
    DefaultTestFixture()
    {
        initialize_mock_arduino();
    }

};
#endif //TEENSY_X86_SD_STUBS_DEFAULT_TEST_FIXTURE_H
