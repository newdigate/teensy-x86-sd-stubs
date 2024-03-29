/*

 SD - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
		  (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics


 This library provides four key benefits:

   * Including `SD.h` automatically creates a global
	 `SD` object which can be interacted with in a similar
	 manner to other standard global objects like `Serial` and `Ethernet`.

   * Boilerplate initialisation code is contained in one method named 
	 `begin` and no further objects need to be created in order to access
	 the SD card.

   * Calls to `open` can supply a full path name including parent 
	 directories which simplifies interacting with files in subdirectories.

   * Utility methods are provided to determine whether a file exists
	 and to create a directory heirarchy.


  Note however that not all functionality provided by the underlying
  sdfatlib library is exposed.

 */

/*

  Implementation Notes

  In order to handle multi-directory path traversal, functionality that 
  requires this ability is implemented as callback functions.

  Individual methods call the `walkPath` function which performs the actual
  directory traversal (swapping between two different directory/file handles
  along the way) and at each level calls the supplied callback function.

  Some types of functionality will take an action at each level (e.g. exists
  or make directory) which others will only take an action at the bottom
  level (e.g. open).

 */
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "SD.h"
namespace SDLib {

// Used by `getNextPathComponent`
#define MAX_COMPONENT_LEN 12 // What is max length?
#define PATH_COMPONENT_BUFFER_LEN MAX_COMPONENT_LEN+1

bool getNextPathComponent(const char *path, unsigned int *p_offset,
			  char *buffer) {
  /*

	Parse individual path components from a path.

	  e.g. after repeated calls '/foo/bar/baz' will be split
		   into 'foo', 'bar', 'baz'.

	This is similar to `strtok()` but copies the component into the
	supplied buffer rather than modifying the original string.


	`buffer` needs to be PATH_COMPONENT_BUFFER_LEN in size.

	`p_offset` needs to point to an integer of the offset at
	which the previous path component finished.

	Returns `true` if more components remain.

	Returns `false` if this is the last component.
	  (This means path ended with 'foo' or 'foo/'.)

   */

  // TODO: Have buffer local to this function, so we know it's the
  //       correct length?

  int bufferOffset = 0;

  int offset = *p_offset;

  // Skip root or other separator
  if (path[offset] == '/') {
	offset++;
  }
  
  // Copy the next next path segment
  while (bufferOffset < MAX_COMPONENT_LEN
	 && (path[offset] != '/')
	 && (path[offset] != '\0')) {
	buffer[bufferOffset++] = path[offset++];
  }

  buffer[bufferOffset] = '\0';

  // Skip trailing separator so we can determine if this
  // is the last component in the path or not.
  if (path[offset] == '/') {
	offset++;
  }

  *p_offset = offset;

  return (path[offset] != '\0');
}



bool walkPath(const char *filepath, SdFile& parentDir,
		 bool (*callback)(SdFile& parentDir,
					 const char *filePathComponent,
					 bool isLastComponent,
					 void *object),
		 void *object = NULL) {
  /*

	 When given a file path (and parent directory--normally root),
	 this function traverses the directories in the path and at each
	 level calls the supplied callback function while also providing
	 the supplied object for context if required.

	   e.g. given the path '/foo/bar/baz'
			the callback would be called at the equivalent of
		'/foo', '/foo/bar' and '/foo/bar/baz'.

	 The implementation swaps between two different directory/file
	 handles as it traverses the directories and does not use recursion
	 in an attempt to use memory efficiently.

	 If a callback wishes to stop the directory traversal it should
	 return false--in this case the function will stop the traversal,
	 tidy up and return false.

	 If a directory path doesn't exist at some point this function will
	 also return false and not subsequently call the callback.

	 If a directory path specified is complete, valid and the callback
	 did not indicate the traversal should be interrupted then this
	 function will return true.

   */


  SdFile subfile1;
  SdFile subfile2;

  char buffer[PATH_COMPONENT_BUFFER_LEN]; 

  unsigned int offset = 0;

  SdFile *p_parent;
  SdFile *p_child;

  SdFile *p_tmp_sdfile;  
  
  p_child = &subfile1;
  
  p_parent = &parentDir;

  while (true) {

	bool moreComponents = getNextPathComponent(filepath, &offset, buffer);

	bool shouldContinue = callback((*p_parent), buffer, !moreComponents, object);

	if (!shouldContinue) {
	  // TODO: Don't repeat this code?
	  // If it's one we've created then we
	  // don't need the parent handle anymore.
	  if (p_parent != &parentDir) {
		(*p_parent).close();
	  }
	  return false;
	}

	if (!moreComponents) {
	  break;
	}

	bool exists = (*p_child).open(p_parent, buffer, O_RDONLY);

	// If it's one we've created then we
	// don't need the parent handle anymore.
	if (p_parent != &parentDir) {
	  (*p_parent).close();
	}

	// Handle case when it doesn't exist and we can't continue...
	if (exists) {
	  // We alternate between two file handles as we go down
	  // the path.
	  if (p_parent == &parentDir) {
		p_parent = &subfile2;
	  }

	  p_tmp_sdfile = p_parent;
	  p_parent = p_child;
	  p_child = p_tmp_sdfile;
	} else {
	  return false;
	}
  }
  
  if (p_parent != &parentDir) {
	(*p_parent).close(); // TODO: Return/ handle different?
  }

  return true;
}



/*

   The callbacks used to implement various functionality follow.

   Each callback is supplied with a parent directory handle,
   character string with the name of the current file path component,
   a flag indicating if this component is the last in the path and
   a pointer to an arbitrary object used for context.

 */

bool callback_pathExists(SdFile& parentDir, const char *filePathComponent,
				bool /* isLastComponent */, void * /* object */) {
  /*

	Callback used to determine if a file/directory exists in parent
	directory.

	Returns true if file path exists.

  */
  SdFile child;

  bool exists = child.open(&parentDir, filePathComponent, O_RDONLY);
  
  if (exists) {
	 child.close();
  }
  
  return exists;
}

bool callback_makeDirPath(
		SdFile& parentDir,
		const char *filePathComponent,
		bool isLastComponent,
		void *object) {
  /*

	Callback used to create a directory in the parent directory if
	it does not already exist.

	Returns true if a directory was created or it already existed.

  */
	bool result = false;
  SdFile child;
  
  result = callback_pathExists(parentDir, filePathComponent, isLastComponent, object);
  if (!result) {
	result = child.makeDir(&parentDir, filePathComponent);
  } 
  
  return result;
}


  /*

boolean callback_openPath(SdFile& parentDir, char *filePathComponent, 
			  boolean isLastComponent, void *object) {

	Callback used to open a file specified by a filepath that may
	specify one or more directories above it.

	Expects the context object to be an instance of `SDClass` and
	will use the `file` property of the instance to open the requested
	file/directory with the associated file open mode property.

	Always returns true if the directory traversal hasn't reached the
	bottom of the directory heirarchy.

	Returns false once the file has been opened--to prevent the traversal
	from descending further. (This may be unnecessary.)

  if (isLastComponent) {
	SDClass *p_SD = static_cast<SDClass*>(object);
	p_SD->file.open(parentDir, filePathComponent, p_SD->fileOpenMode);
	if (p_SD->fileOpenMode == FILE_WRITE) {
	  p_SD->file.seekSet(p_SD->file.fileSize());
	}
	// TODO: Return file open result?
	return false;
  }
  return true;
}
  */



bool callback_remove(SdFile& parentDir, const char *filePathComponent,
			bool isLastComponent, void * /* object */) {
  if (isLastComponent) {
	return SdFile::remove(&parentDir, filePathComponent);
  }
  return true;
}

bool callback_rmdir(SdFile& parentDir, const char *filePathComponent,
			bool isLastComponent, void * /* object */) {
  if (isLastComponent) {
	SdFile f;
	if (!f.open(&parentDir, filePathComponent, O_READ)) return false;
	return f.rmDir();
  }
  return true;
}



/* Implementation of class used to create `SDCard` object. */



bool SDClass::begin(uint8_t csPin) {
  /*

	Performs the initialisation required by the sdfatlib library.

	Return true if initialization succeeds, false otherwise.

   */
  return (_sdCardFolderLocation.length() > 0 || (_fileData != NULL && _fileSize > 0)) ;
}

bool SDClass::begin(uint32_t clock, uint8_t csPin) {
    return (_sdCardFolderLocation.length() > 0 || (_fileData != NULL && _fileSize > 0));
}

// this little helper is used to traverse paths
SdFile SDClass::getParentDir(const char *filepath, int *index) {
  // get parent directory
  SdFile d1 = root; // start with the mostparent, root!
  SdFile d2;

  // we'll use the pointers to swap between the two objects
  SdFile *parent = &d1;
  SdFile *subdir = &d2;
  
  const char *origpath = filepath;

  while (strchr(filepath, '/')) {

	// get rid of leading /'s
	if (filepath[0] == '/') {
	  filepath++;
	  continue;
	}

	if (! strchr(filepath, '/')) {
	  // it was in the root directory, so leave now
	  break;
	}

	// extract just the name of the next subdirectory
	uint8_t idx = strchr(filepath, '/') - filepath;
	if (idx > 12)
	  idx = 12;    // dont let them specify long names
	char subdirname[13];
	strncpy(subdirname, filepath, idx);
	subdirname[idx] = 0;

	// close the subdir (we reuse them) if open
	subdir->close();
	if (! subdir->open(parent, subdirname, O_READ)) {
	  // failed to open one of the subdirectories
	  return SdFile();
	}
	// move forward to the next subdirectory
	filepath += idx;

	// we reuse the objects, close it.
	parent->close();

	// swap the pointers
	SdFile *t = parent;
	parent = subdir;
	subdir = t;
  }

  *index = (int)(filepath - origpath);
  // parent is now the parent diretory of the file!
  return *parent;
}


File SDClass::open(const char *filepath, uint8_t mode) {
    AbstractFile *result;
    if (_useMockData) {
        result = new InMemoryFile(filepath, _fileData, _fileSize, mode  );
    } else {
        std::string pathString = std::string(filepath);
        std::string fileNameString = std::string(filepath);
        size_t last_slash_idx = pathString.rfind('/');
        char *path = "";
        if (std::string::npos != last_slash_idx)
        {
            fileNameString = fileNameString.substr(last_slash_idx+1, strlen(filepath)-last_slash_idx-1);
            std::string temppath = pathString.substr(0, last_slash_idx);
            path = new char[temppath.length() + 1] {0};
            memcpy(path, temppath.c_str(), temppath.length());
        }
        result = new LinuxFile(fileNameString.c_str(), path, mode, *this);
    }
    return File(result);
}

bool SDClass::exists(const char *filepath) {
    if (_useMockData)
    	return true;

    const std::string path = _sdCardFolderLocation + "/" + std::string(filepath);
    const char *pathCstr = path.c_str();
    std::fstream file(pathCstr);
    bool isFile = (bool)file;
    if (isFile)
        return true;

    bool is_Directory = LinuxFile::is_directory (pathCstr);
    return is_Directory;
}

std::string SDClass::getSDCardFolderPath() {
    return _sdCardFolderLocation;
}

void SDClass::setSDCardFolderPath(std::string path, bool createDirectoryIfNotAlreadyExisting) {
	_useMockData = false;
	_sdCardFolderLocation = "";
	if (createDirectoryIfNotAlreadyExisting && !exists(path) ) {
		mkdir(path);
	}

    _sdCardFolderLocation = path;
}

bool SDClass::mkdir(const char *filepath) {
    std::string path;
	
	if (_sdCardFolderLocation.size() == 0)
		path = std::string(filepath);
	else
		path = _sdCardFolderLocation + "/" + std::string(filepath);

    if (!exists(path.c_str())) {
        std::string cmd = std::string("mkdir -p \"") + std::string(path) + "\"";
        int result = system(cmd.c_str());
    	if (result != 0) Serial.printf("Unable to mkdir '%s'\n", filepath);
        return result == 0;
    }
    return true;
}

bool SDClass::rmdir(const char *filepath) {
    if (_sdCardFolderLocation.size() == 0)
        return true;

    std::string path = _sdCardFolderLocation + "/" + std::string(filepath);
    if (exists(filepath)) {
        std::string cmd = std::string("rm -rf ") + std::string(path);
        system(cmd.c_str());
        return true;
    }
    return true;
}

bool SDClass::remove(const char *filepath) {
    if (_sdCardFolderLocation.size() == 0)
        return false;
    
    std::string path = _sdCardFolderLocation + "/" + std::string(filepath);
    if (exists(filepath)) {
        std::string cmd = std::string("rm -rf ") + std::string(path);
        system(cmd.c_str());
        return true;
    }
    return true;
}

SDClass SD;


AbstractFile::AbstractFile(const char *fileName) : _fileName(fileName) {}
};
