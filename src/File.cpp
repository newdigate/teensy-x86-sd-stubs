#include "SD.h"

// returns a pointer to the file name
const char *File::name(void) {
    return file->_fileName;
}

File::File(AbstractFile *abs) : file(abs) {

}

File::File() {
    file = nullptr;
}

int File::read(void *buf, uint16_t nbyte) {
    return file->read(buf, nbyte);
}

bool File::seek(uint32_t pos) {
    return file->seek(pos);
}

uint32_t File::position() {
    return file->position();
}
bool File::truncate(uint64_t size) {
    return file->truncate();
}


uint32_t File::size() {
    return file->size();
}

void File::close() {
    if (file != nullptr) {
        file->close();
        file = nullptr;
    } else 
    {
        Serial.println("WARNING: File::close() - file is already closed!!!!");
    }
}

bool File::isDirectory(void) {
    return file->isDirectory();
}

int File::read() {
    return file->read();
}

int File::peek() {
    return file->peek();
}

int File::available() {
    return file != nullptr && file->available();
}

void File::flush() {
    return file->flush();
}

File::operator bool() {
    if (file == nullptr) return false;
    bool result = file->operator bool();
    return result;
}

size_t File::write(const uint8_t *buf, size_t size) {
    return file->write(buf, size);
}

size_t File::write(uint8_t ch) {
    return file->write(ch);
}

File::File(const File &f) : file(f.file) {

}

File File::openNextFile(void) {
    return file->openNextFile();
}
