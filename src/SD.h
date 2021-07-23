#ifndef __SD_H__
#define __SD_H__

#include "Arduino.h"
#include "utility/SdFat.h"
#include "Print.h"
#include <cstdio>
#include "utility/SdFatUtil.h"
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <cstdint>

using namespace std;

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT)
namespace SDLib {
    class File;

    class AbstractFile  {
    public:
        uint32_t _size = -1;
        bool _isDirectory;
        const char *_fileName;

        virtual bool isDirectory(void) = 0;
        virtual int write(uint8_t) = 0;
        virtual int write(const uint8_t *buf, size_t size) = 0;
        virtual int read() = 0;
        virtual int peek() = 0;
        virtual int available() = 0;
        virtual void flush() = 0;
        virtual int read(void *buf, uint16_t nbyte) = 0;
        virtual bool seek(uint32_t pos) = 0;
        virtual uint32_t position() = 0;
        virtual uint32_t size() = 0;
        virtual void close() = 0;
        virtual operator bool() = 0;
        virtual File openNextFile(void) = 0;
    };

class File {
protected:
    AbstractFile *file;

public:

    File(AbstractFile *abs);

    File(const File& f);

    File();

    int write(uint8_t ch);

    int write(const uint8_t *buf, size_t size);
    int read();
    int peek();
    int available();
    void flush();
    int read(void *buf, uint16_t nbyte);
    bool seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    const char * name();

    bool isDirectory(void);
    File openNextFile(void);
};

class InMemoryFile : public AbstractFile {
private:
    char* _data;
    uint32_t _position;
public:
    InMemoryFile(const char *name, char *data, uint32_t size, uint8_t mode = O_READ);
    InMemoryFile(void);      // 'empty' constructor
    bool isDirectory(void) override;
    int write(uint8_t) override;
    int write(const uint8_t *buf, size_t size) override;
    int read() override;
    int peek() override;
    int available() override;
    void flush() override;
    int read(void *buf, uint16_t nbyte) override;
    bool seek(uint32_t pos) override;
    uint32_t position() override;
    uint32_t size() override;
    void close() override;
    explicit operator bool() override {
         return _size >= 0;
    }
    File openNextFile(void) override;

};

class LinuxFile : public AbstractFile {
private:
    const char * localFileName;
    char * path;
    std::fstream mockFile = std::fstream();
    DIR *dp = NULL;
public:
    LinuxFile(const char *name, uint8_t mode = O_READ);
    LinuxFile(void);      // 'empty' constructor
    static std::streampos fileSize( const char* filePath );
    static bool is_directory( const char* pzPath );
    int write(uint8_t) override;
    int write(const uint8_t *buf, size_t size) override;
    int read() override;
    int peek() override;
    int available() override;
    void flush() override;
    int read(void *buf, uint16_t nbyte) override;
    bool seek(uint32_t pos) override;
    uint32_t position() override;
    uint32_t size() override;
    void close() override;
    explicit operator bool() override {
        return (mockFile.is_open() ||  isDirectory());
    }
    bool isDirectory(void) override {
        return is_directory(localFileName);
    }
    File openNextFile(void) override;
};

class SDClass {
private:
  // These are required for initialisation and use of sdfatlib
    Sd2Card card;
    SdVolume volume;
    SdFile root;

    // my quick&dirty iterator, should be replaced
    SdFile getParentDir(const char *filepath, int *indx);

    static std::string _sdCardFolderLocation;
    static bool _useMockData;
    static char *_fileData;
    static uint32_t _fileSize;

    public:

    static std::string getSDCardFolderPath();

    static void setSDCardFolderPath(std::string path);
    
    static void setSDCardFileData(char *data, uint32_t size) {
        _fileData = data;
        _fileSize = size;
        _useMockData = true;
    }

    // This needs to be called to set up the connection to the SD card
    // before other methods are used.
    bool begin(uint8_t csPin = 0);
    bool begin(uint32_t clock, uint8_t csPin);

    // Open the specified file/directory with the supplied mode (e.g. read or
    // write, etc). Returns a File object for interacting with the file.
    // Note that currently only one file can be open at a time.
    File open(const char *filename, uint8_t mode = FILE_READ);
    File open(const std::string &filename, uint8_t mode = FILE_READ) { return open( filename.c_str(), mode ); }

    // Methods to determine if the requested file path exists.
    bool exists(const char *filepath);
    bool exists(const std::string &filepath) { return exists(filepath.c_str()); }

    // Create the requested directory heirarchy--if intermediate directories
    // do not exist they will be created.
    bool mkdir(const char *filepath);
    bool mkdir(const std::string &filepath) { return mkdir(filepath.c_str()); }

    // Delete the file.
    bool remove(const char *filepath);
    bool remove(const std::string &filepath) { return remove(filepath.c_str()); }

    bool rmdir(const char *filepath);
    bool rmdir(const std::string &filepath) { return rmdir(filepath.c_str()); }

private:

    // This is used to determine the mode used to open a file
    // it's here because it's the easiest place to pass the
    // information through the directory walking function. But
    // it's probably not the best place for it.
    // It shouldn't be set directly--it is set via the parameters to `open`.
    int fileOpenMode;

    friend class File;
    friend bool callback_openPath(SdFile&, const char *, bool, void *);
};

extern SDClass SD;

};

// We enclose File and SD classes in namespace SDLib to avoid conflicts
// with others legacy libraries that redefines File class.

// This ensure compatibility with sketches that uses only SD library
using namespace SDLib;

// This allows sketches to use SDLib::File with other libraries (in the
// sketch you must use SDFile instead of File to disambiguate)
typedef SDLib::File    SDFile;
typedef SDLib::SDClass SDFileSystemClass;
#define SDFileSystem   SDLib::SD

#endif
