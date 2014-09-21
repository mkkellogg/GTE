/*
 * class: FileSystemIX
 *
 * author: Mark Kellogg
 *
 * Implementation of UNIX/Linux/OSX specific file system mechanics
 *
 */

#ifndef _FILE_SYSTEM_IX_H_
#define _FILE_SYSTEM_IX_H_

//forward declarations

#include "filesystem.h"
#include <string>

class FileSystemIX : public FileSystem
{
	friend class FileSystem;

	protected:

	FileSystemIX();
	~FileSystemIX();

	public:

	std::string ConcatenatePaths(const std::string& pathA, const std::string& pathB) const;
    std::string GetBasePath(const std::string& path) const;
	std::string GetPathFromIXPath(const std::string& path) const;
	std::string FixupPath(const std::string& path) const;
};

#endif
