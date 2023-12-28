#include "SD.h"

InMemoryFile::InMemoryFile(const char *name, char *data, uint32_t size, uint8_t mode) : AbstractFile(name) {
    _fileName = name;
    _data = data;
    _size = size;
    _position = 0;
    _isOpen = true;
}

InMemoryFile::InMemoryFile(void) : AbstractFile("n/a") {
  _size = -1;
  _position = 0;
  _isOpen = true;
}

// a directory is a special type of file
bool InMemoryFile::isDirectory(void) {
  return _isDirectory;
}

size_t InMemoryFile::write(uint8_t val) {
    return write(&val, 1);
}

size_t InMemoryFile::write(const uint8_t *buf, size_t size) {
    return 0;
}

int InMemoryFile::peek() {
    return 0;
}

int InMemoryFile::read() {
    if (_position >= _size) {
        std::cout << "!!! CRITICAL: read outside bounds of file...";
        return 0;
    }

    int result = _data[_position];
    _position++;
    return result;
}

// buffered read for more efficient, high speed reading
int InMemoryFile::read(void *buf, uint16_t nbyte) {
    char * target = (char*)buf;
    for (int i=0; i < nbyte; i++) {
        if  (_position >= _size)
            return i;
        int byteRead = read();
        target[i] = static_cast<char>(byteRead);
    }
    return nbyte;
}

int InMemoryFile::available() {
    return _isOpen & (_position < _size);
}

void InMemoryFile::flush() {
}
bool InMemoryFile::seek(uint32_t pos) {
    if (pos < _size) {
        _position = pos;
        return true;
    }
    return false;
}

uint32_t InMemoryFile::position() {
    return _position;
}

uint32_t InMemoryFile::size() {
    return _size;
}

void InMemoryFile::close() {
    if (_isOpen) {
        _isOpen = false;
    }
}

bool InMemoryFile::truncate(uint64_t size) {
    _size=0;
    _position = 0;
    return true;
}

File InMemoryFile::openNextFile(void) {
    return File { new InMemoryFile()};
}

