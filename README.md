# teensy-x86-sd-stubs
[![Ubuntu-x64](https://github.com/newdigate/teensy-x86-sd-stubs/actions/workflows/ubuntu-x86.yml/badge.svg)](https://github.com/newdigate/teensy-x86-sd-stubs/actions/workflows/ubuntu-x86.yml)

mock SD for teensy

## eco-system
[cores](https://github.com/newdigate/teensy-x86-stubs)

##### initialization
* To map the root directory of the mock microSD card, file to a char* array, next SD file read access will return the data in the buffer
``` c++
    void SD::setSDCardFolderPath(std::string path, bool createDirectoryIfNotAlreadyExisting = false);

    SD.setSDCardFolderPath("/Volume/SDcard1", true);
```

* To map the data in a mock file to a char* array, next SD file read access will return the data in the buffer
``` c++ 
    char *buffer = "blah blah blah blah blah";
    SD.setSDCardFileData(buffer, strlen(buffer));
```

##### main.cpp
``` c++
#include <Arduino.h>
#include <SD.h>

using namespace std;

int main(int argc, char **argv){
    std::cout << "starting app...\n";
    initialize_mock_arduino();

    char *buffer = "blah blah blah blah blah";
    SD.setSDCardFileData(buffer, strlen(buffer));

    File f = SD.open("abcdefg.123");

    char *output = new char[1000];
    int bytesRead = f.read(output, 1000);

    std::cout << bytesRead << " bytes read: \n" << output;
}
```
##### output
```
starting app...
24 bytes read: 
blah blah blah blah blah
```
