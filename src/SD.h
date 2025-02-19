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

#define BUILTIN_SDCARD 254

#define FILE_READ O_READ
#define FILE_WRITE (O_READ | O_WRITE | O_CREAT | O_APPEND)
namespace SDLib {
    class File;
    class SDClass;
    extern SDClass SD;

    class AbstractFile : public Stream {
    public:
        int32_t _size = -1;
        bool _isDirectory;
        const char *_fileName;

        explicit AbstractFile(const char *fileName);

        virtual bool isDirectory() = 0;
        int read() override = 0;
        virtual int read(void *buf, uint32_t nbyte) = 0;
        virtual bool seek(uint32_t pos) = 0;
        virtual uint32_t position() = 0;
        virtual uint32_t size() = 0;
        virtual bool truncate(uint64_t size=0) = 0;
        virtual void close() = 0;
        virtual operator bool() = 0;
        virtual File openNextFile(void) = 0;

    };

class File : public Stream {
protected:
    AbstractFile *file;

public:

    explicit File(AbstractFile *abs);

    File(const File& f);

    File();

    size_t write(uint8_t ch) override;

    size_t write(const uint8_t *buf, size_t size) override;
    int read() override;
    int peek() override;
    int available() override;
    void flush() override;
    bool truncate(uint64_t size=0);
    int read(void *buf, uint32_t nbyte);
    bool seek(uint32_t pos);
    uint32_t position();
    uint32_t size();
    void close();
    operator bool();
    const char * name();
    bool isDirectory();
    File openNextFile();
    void rewindDirectory() {}
};

class InMemoryFile : public AbstractFile {
private:
    char* _data;
    uint32_t _position;
    bool _isOpen;
public:
    InMemoryFile(const char *name, char *data, uint32_t size, uint8_t mode = O_READ);
    InMemoryFile(void);      // 'empty' constructor
    bool isDirectory(void) override;
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int read() override;
    int peek() override;
    int available() override;
    void flush() override;
    bool truncate(uint64_t size) override;
    int read(void *buf, uint32_t nbyte) override;
    bool seek(uint32_t pos) override;
    uint32_t position() override;
    uint32_t size() override;
    void close() override;
    explicit operator bool() override {
         return _size >= 0;
    }
    File openNextFile() override;

};

class LinuxFile : public AbstractFile {
private:
    const char * localFileName;
    char * localPath;
    const char * _path;
    std::fstream mockFile = std::fstream();
    DIR *dp = NULL;
public:
    LinuxFile(const char *name, const char *path, uint8_t mode = O_READ, SDClass &sd = SD);
    LinuxFile(SDClass &sd = SD);

    static std::streampos fileSize( const char* filePath );
    static bool is_directory( const char* pzPath );
    size_t write(uint8_t) override;
    size_t write(const uint8_t *buf, size_t size) override;
    int read() override;
    int peek() override;
    int available() override;
    void flush() override;
    bool truncate(uint64_t size) override;
    int read(void *buf, uint32_t nbyte) override;
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
    SDClass &_sd;
};

class SDClass {
private:
  // These are required for initialisation and use of sdfatlib
    Sd2Card card;
    SdVolume volume;
    SdFile root;

    // my quick&dirty iterator, should be replaced
    SdFile getParentDir(const char *filepath, int *indx);

    std::string _sdCardFolderLocation;
    bool _useMockData;
    char *_fileData;
    uint32_t _fileSize;

public:
    SDClass() {

    }

    SDClass(std::string sdCardFolderLocation) : 
        _sdCardFolderLocation(sdCardFolderLocation) 
    {

    }

    std::string getSDCardFolderPath();

    void setSDCardFolderPath(std::string path, bool createDirectoryIfNotAlreadyExisting = false);
    
    void setSDCardFileData(char *data, uint32_t size) {
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
