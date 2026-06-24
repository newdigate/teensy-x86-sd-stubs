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
    // Routes through the buffer write below; in-memory mode is read-only.
    return write(&val, 1);
}

size_t InMemoryFile::write(const uint8_t *buf, size_t size) {
    // In-memory mode is read-only: writes are intentionally a no-op and
    // report zero bytes written. The mock backs a single read-only buffer
    // supplied via SD.setSDCardFileData(); use folder-backed mode for writes.
    return 0;
}

int InMemoryFile::peek() {
    return 0;
}

int InMemoryFile::read() {
    // _size is int32_t with -1 as the empty/sentinel value; treat that as
    // empty rather than comparing _position against a huge unsigned value.
    if (_size < 0 || _position >= static_cast<uint32_t>(_size)) {
        std::cout << "!!! CRITICAL: read outside bounds of file...";
        return 0;
    }

    int result = _data[_position];
    _position++;
    return result;
}

// buffered read for more efficient, high speed reading
int InMemoryFile::read(void *buf, uint32_t nbyte) {
    char * target = (char*)buf;
    for (uint32_t i=0; i < nbyte; i++) {
        // _size is int32_t with -1 as the empty/sentinel value; treat that as
        // empty rather than comparing _position against a huge unsigned value.
        if (_size < 0 || _position >= static_cast<uint32_t>(_size))
            return i;
        int byteRead = read();
        target[i] = static_cast<char>(byteRead);
    }
    return nbyte;
}

int InMemoryFile::available() {
    return _isOpen && (_size >= 0) && (_position < static_cast<uint32_t>(_size));
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

