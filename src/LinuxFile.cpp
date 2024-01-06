/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#include "SD.h"

#include <string>
#include <unistd.h>

LinuxFile::LinuxFile(const char *name, const char *path, uint8_t mode, SDClass &sd) : AbstractFile(name), _sd(sd) {
    std::string actualFileName;
    if (strlen(path)==0) {
        actualFileName = sd.getSDCardFolderPath() + std::string("/") + std::string(name);
    } else
        actualFileName = sd.getSDCardFolderPath() + std::string("/") + std::string(path) + std::string("/") + std::string(name);

    localFileName = new char[actualFileName.length() + 1] {0};
    memcpy((char*)localFileName, actualFileName.c_str(), actualFileName.length());
    
    size_t last_slash_idx = actualFileName.rfind('/');
    if (std::string::npos != last_slash_idx)
    {
        std::string temppath = actualFileName.substr(0, last_slash_idx);
        localPath = new char[temppath.length() + 1] {0};
        memcpy(localPath, temppath.c_str(), temppath.length());
    }
    _path = path;

    // cout << actualFileName;
    if (!is_directory(localFileName) ) {

        std::iostream::openmode flags = static_cast<std::iostream::openmode>(0);
        if ((mode & O_READ) == O_READ)
            flags |= std::fstream::in;

        if ((mode & O_WRITE) == O_WRITE)
            flags |= std::fstream::out;

        if ((mode & O_APPEND) == O_APPEND)
            flags |= std::fstream::app;

        if ((mode & O_TRUNC) == O_TRUNC)
            flags |= std::fstream::trunc;

        mockFile.open(actualFileName, flags);
        if (!mockFile) {
            std::cout << "Not able to open " << actualFileName;
        }
        _size = fileSize(actualFileName.c_str());
    }
    _fileName = name;
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

size_t LinuxFile::write(uint8_t val) {
    return write(&val, 1);
}

size_t LinuxFile::write(const uint8_t *buf, size_t size) {
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
    mockFile.seekp(pos, std::ios::beg);
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

int LinuxFile::read(void *buf, uint32_t nbyte) {
    mockFile.read((char *)buf, nbyte);
    int bytesRead = mockFile.gcount();
    return bytesRead;
}

File LinuxFile::openNextFile(void) {
    bool isCurrentFileADirectory = is_directory(localFileName);

    struct dirent *entry;
    
    if (!dp) 
        dp = opendir(isCurrentFileADirectory? localFileName : localPath);

    if (dp == NULL) {
        perror("opendir: Path does not exist or could not be read.");
        return File(new InMemoryFile());
    }

    if (isCurrentFileADirectory){
        while (true) {
            entry = readdir(dp);
        
            if (entry != NULL) {
                auto l = _sd.getSDCardFolderPath().length();
                std::string lfnString = std::string( localFileName );
                std::string final = lfnString.substr(l+1, lfnString.length()-l-1);
                File f = File(new LinuxFile(entry->d_name, final.c_str(), O_READ, this->_sd));
                return f;
            } else 
                break;
        }
    }

    
    //closedir(dp);

    return File( new InMemoryFile());
}

bool LinuxFile::truncate(uint64_t size) {
    if (::truncate(_fileName, size) != 0)
        return false;
    return true;
}