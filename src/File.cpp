#include "SD.h"

// returns a pointer to the file name
const char *File::name(void) {
    return file->_fileName;
}

File::File(AbstractFile *abs) : file(abs) {

}

File::File() {

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

uint32_t File::size() {
    return file->size();
}

void File::close() {
    return file->close();
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
    return file->available();
}

void File::flush() {
    return file->flush();
}

File::operator bool() {
    if (file == nullptr) return false;
    return true;
}

int File::write(const uint8_t *buf, size_t size) {
    file->write(buf, size);
}

int File::write(uint8_t ch) {
    file->write(ch);
}

File::File(const File &f) : file(f.file) {

}
