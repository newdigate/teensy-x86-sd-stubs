/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include "SD.h"
#include <dirent.h>

using namespace std;

LinuxFile::LinuxFile(const char *name, uint8_t mode) {
    std::string actualFileName = SDClass::getSDCardFolderPath() + std::string("/") + std::string(name);
    localFileName = actualFileName.c_str();
    // cout << actualFileName;
    switch (mode) {
        case O_READ : mockFile.open(actualFileName); break;
        case O_WRITE : mockFile.open(actualFileName, std::fstream::out | std::fstream::app); break;
        default:
            break;
    }
    _fileName = name;
    _size = fileSize(actualFileName.c_str());
}

std::streampos LinuxFile::fileSize( const char* filePath ){
    std::streampos fsize = 0;
    std::ifstream file( filePath, std::ios::binary );

    fsize = file.tellg();
    file.seekg( 0, std::ios::end );
    fsize = file.tellg() - fsize;
    file.close();

    return fsize;
}

bool LinuxFile::is_directory( const char* pzPath )
{
    if ( pzPath == NULL) return false;

    DIR *pDir;
    bool bExists = false;

    pDir = opendir (pzPath);

    if (pDir != NULL)
    {
        bExists = true;
        (void) closedir (pDir);
    }

    return bExists;
}

int LinuxFile::write(uint8_t val) {
    return write(&val, 1);
}

int LinuxFile::write(const uint8_t *buf, size_t size) {
    size_t t;
    if (!mockFile.is_open()) {
        return 0;
    }

    _size += size;
    char * memblock = (char *)buf;
    mockFile.write(memblock, size);

    return t;
}

int LinuxFile::read() {
    char p[1];
    mockFile.read(p, 1);
    return p[0];
}

int LinuxFile::peek() {
    if (! mockFile.is_open())
        return 0;

    return mockFile.peek();
}

int LinuxFile::available() {
    if (! mockFile.is_open()) return 0;
    uint32_t p = position();
    long s = size();
    if (p > s) return 0;

    long n = s - p;

    return n > 0X7FFF ? 0X7FFF : n;
}

void LinuxFile::flush() {
    if (mockFile.is_open())
        mockFile.flush();
}

bool LinuxFile::seek(uint32_t pos) {
    if (! mockFile.is_open()) return false;
    mockFile.seekp(pos, std::ios::cur);
    return true;
}

uint32_t LinuxFile::position() {
    if (! mockFile.is_open()) return -1;
    return mockFile.tellp();
}

uint32_t LinuxFile::size() {
    return _size;
}

void LinuxFile::close() {
    if (mockFile.is_open())
        mockFile.close();
}

int LinuxFile::read(void *buf, uint16_t nbyte) {
    char *bbb = (char *)buf;
    mockFile.read(bbb, nbyte);
    return nbyte;
}
